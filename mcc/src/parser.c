// The parsing rules defined here operate in conjuncting with the parser
// instance. Each rule matches on tokens and returns a corresponding AST node on
// success, NULL on failure.
//
// The rules are layed out to require only 1 token lookahead.
//
// Precedence climbing is used, see:
// https://en.wikipedia.org/wiki/Operator-precedence_parser#Precedence_climbing_method
//
// The parser's `error` field is populate with errors accordingly.

#include "mcc/parser.h"

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "mcc/lexer.h"

// ---------------------------------------------------------------- Parser

// These shorthands are introduced to enhance code readability. The convention
// is to always have the parser instance accessible via `parser`.
#define accept(token) parser_accept(parser, token)
#define expect(token) parser_expect(parser, token)
#define error(...) parser_error_msg(parser, MCC_PARSER_ERROR_PARSE_ERROR, parser->lexeme.sloc, __VA_ARGS__)

struct parser {
	struct mcc_lexer lexer;
	struct mcc_lexeme lexeme;

	// Filepath used for prefixing error messages.
	const char *filepath;

	enum mcc_parser_error error;
	char error_msg[1024];
};

// Sets the parser's `error` field iff it is MCC_PARSER_ERROR_NONE. This is used
// to prevent errors from overwriting another.
static void parser_error(struct parser *parser, enum mcc_parser_error error)
{
	assert(parser);

	if (parser->error == MCC_PARSER_ERROR_NONE) {
		parser->error = error;
	}
}

// Same as `parser_error` but also sets the `error_msg` field. Arguments are
// forwarded to `vsnprintf`.
static void
parser_error_msg(struct parser *parser, enum mcc_parser_error error, struct mcc_sloc sloc, const char *format, ...)
{
	assert(parser);
	assert(format);

	if (parser->error != MCC_PARSER_ERROR_NONE) {
		return;
	}

	parser->error = error;

	int prefix_length = snprintf(parser->error_msg, sizeof(parser->error_msg),
	                             "%s:%d:%d: error: ", parser->filepath, sloc.line, sloc.column);

	va_list args;
	va_start(args, format);
	vsnprintf(parser->error_msg + prefix_length, sizeof(parser->error_msg) - prefix_length, format, args);
	va_end(args);
}

// Grabs the next lexeme from the lexer, populating the parser's `lexeme` field.
// Sets the `error` field on error.
static void parser_next(struct parser *parser)
{
	parser->lexeme = mcc_lexer_lex(&parser->lexer);

	switch (parser->lexeme.token) {
	case MCC_TOKEN_ERROR:
		parser_error_msg(parser, MCC_PARSER_ERROR_LEXER_ERROR, parser->lexeme.sloc, "lexer: %s",
		                 mcc_lexer_error_to_string(parser->lexer.error));
		break;
	case MCC_TOKEN_UNKNOWN:
		parser_error_msg(parser, MCC_PARSER_ERROR_LEXER_ERROR, parser->lexeme.sloc, "unknown token: '%s'",
		                 parser->lexeme.s_value);
		break;
	default:
		break;
	}
}

// Accepts the given token, advancing the parser.
static bool parser_accept(struct parser *parser, enum mcc_token token)
{
	assert(parser);

	if (parser->lexeme.token != token) {
		return false;
	}

	parser_next(parser);
	return true;
}

// Same as accept, but errors out if the given token does not match.
static bool parser_expect(struct parser *parser, enum mcc_token token)
{
	assert(parser);

	if (parser->lexeme.token != token) {
		error("unexpected '%s', expected '%s'", mcc_token_to_string(parser->lexeme.token),
		      mcc_token_to_string(token));
		return false;
	}

	parser_next(parser);
	return true;
}

// ---------------------------------------------------------------- Operators

static int precedence_from_token(enum mcc_token token)
{
	switch (token) {
	case MCC_TOKEN_PLUS:
		return 1;
	case MCC_TOKEN_ASTERISK:
		return 2;
	default:
		return 0;
	}
}

static int precedence_from_binary_op(enum mcc_ast_binary_op op)
{
	switch (op) {
	case MCC_AST_BINARY_OP_ADD:
	case MCC_AST_BINARY_OP_SUB:
		return 1;
	case MCC_AST_BINARY_OP_MUL:
	case MCC_AST_BINARY_OP_DIV:
		return 2;
	}

	return 0;
}

// Like `parser_accept` but will also take precedence into account. `precedence`
// is updated with the new precedence iff successful.
static bool parser_accept_with_precedence(struct parser *parser, enum mcc_token token, int *precedence)
{
	assert(parser);
	assert(precedence);

	if (parser->lexeme.token != token) {
		return false;
	}

	int token_precedence = precedence_from_token(token);
	if (*precedence > token_precedence) {
		return false;
	}

	*precedence = token_precedence;
	parser_next(parser);
	return true;
}

#define accept_with_precedence(token, precedence) parser_accept_with_precedence(parser, token, precedence)

// ---------------------------------------------------------------- Literals

static struct mcc_ast_literal *parse_literal(struct parser *parser)
{
	assert(parser);

	struct mcc_ast_literal *result = NULL;
	struct mcc_lexeme lexeme = parser->lexeme;

	if (accept(MCC_TOKEN_INT_LITERAL)) {
		result = mcc_ast_new_literal_int(lexeme.i_value);
	} else if (accept(MCC_TOKEN_FLOAT_LITERAL)) {
		result = mcc_ast_new_literal_float(lexeme.f_value);
	} else {
		return NULL;
	}

	if (!result) {
		parser_error(parser, MCC_PARSER_ERROR_ALLOCATION_ERROR);
		return NULL;
	}

	result->node.sloc = lexeme.sloc;
	return result;
}

// ---------------------------------------------------------------- Expressions

static struct mcc_ast_expression *parse_expression_literal(struct parser *);
static struct mcc_ast_expression *parse_expression_parenth(struct parser *);
static struct mcc_ast_expression *parse_expression_binary_op(struct parser *, struct mcc_ast_expression *, int);

static struct mcc_ast_expression *parse_expression(struct parser *parser, int precedence)
{
	assert(parser);

	struct mcc_ast_expression *result = NULL;
	struct mcc_sloc sloc = parser->lexeme.sloc;

	// expression rules
	result = result ? result : parse_expression_literal(parser);
	result = result ? result : parse_expression_parenth(parser);

	if (!result) {
		return NULL;
	}

	// continuation
	while (true) {
		struct mcc_ast_expression *binary_op = parse_expression_binary_op(parser, result, precedence);
		if (!binary_op) {
			break;
		}
		result->node.sloc = sloc;
		result = binary_op;
	}

	result->node.sloc = sloc;
	return result;
}

static struct mcc_ast_expression *parse_expression_literal(struct parser *parser)
{
	assert(parser);

	struct mcc_ast_literal *literal = parse_literal(parser);
	if (!literal) {
		return NULL;
	}

	struct mcc_ast_expression *result = mcc_ast_new_expression_literal(literal);
	if (!result) {
		parser_error(parser, MCC_PARSER_ERROR_ALLOCATION_ERROR);
		mcc_ast_delete_literal(literal);
		return NULL;
	}

	return result;
}

static struct mcc_ast_expression *parse_expression_parenth(struct parser *parser)
{
	assert(parser);

	if (!accept(MCC_TOKEN_PARENTH_LEFT)) {
		return NULL;
	}

	struct mcc_ast_expression *sub = parse_expression(parser, 0);
	if (!sub) {
		error("expression_parenth: expected sub-expression");
		return NULL;
	}

	if (!expect(MCC_TOKEN_PARENTH_RIGHT)) {
		mcc_ast_delete_expression(sub);
		return NULL;
	}

	struct mcc_ast_expression *result = mcc_ast_new_expression_parenth(sub);
	if (!result) {
		parser_error(parser, MCC_PARSER_ERROR_ALLOCATION_ERROR);
		mcc_ast_delete_expression(sub);
		return NULL;
	}

	return result;
}

static struct mcc_ast_expression *
parse_expression_binary_op(struct parser *parser, struct mcc_ast_expression *lhs, int precedence)
{
	assert(parser);

	enum mcc_ast_binary_op op;
	if (accept_with_precedence(MCC_TOKEN_PLUS, &precedence)) {
		op = MCC_AST_BINARY_OP_ADD;
	} else if (accept_with_precedence(MCC_TOKEN_ASTERISK, &precedence)) {
		op = MCC_AST_BINARY_OP_MUL;
	} else {
		return NULL;
	}

	struct mcc_ast_expression *rhs = parse_expression(parser, precedence);
	if (!rhs) {
		error("expression_binary_op: expected rhs expression");
		return NULL;
	}

	struct mcc_ast_expression *result = mcc_ast_new_expression_binary_op(op, lhs, rhs);
	if (!result) {
		parser_error(parser, MCC_PARSER_ERROR_ALLOCATION_ERROR);
		mcc_ast_delete_expression(rhs);
		return NULL;
	}

	// Fix associativity, assuming left-to-right associativity.
	if (rhs->type == MCC_AST_EXPRESSION_TYPE_BINARY_OP &&
	    precedence_from_binary_op(rhs->op) == precedence_from_binary_op(op)) {
		// find the first lhs to replace
		struct mcc_ast_expression *new_rhs = rhs;
		while (new_rhs->lhs->type == MCC_AST_EXPRESSION_TYPE_BINARY_OP &&
		       precedence_from_binary_op(new_rhs->lhs->op) == precedence_from_binary_op(op)) {
			new_rhs = new_rhs->lhs;
		}
		result->rhs = new_rhs->lhs;
		new_rhs->lhs = result;
		return rhs;
	}

	return result;
}

// ---------------------------------------------------------------- Interface

void mcc_parser_result_print_error(FILE *out, struct mcc_parser_result *result)
{
	assert(out);
	assert(result);

	if (*result->error_msg) {
		fprintf(out, "%s\n", result->error_msg);
		return;
	}

	switch (result->error) {
	case MCC_PARSER_ERROR_NONE:
		fputs("no error\n", out);
		break;
	case MCC_PARSER_ERROR_LEXER_ERROR:
		fputs("lexer error\n", out);
		break;
	case MCC_PARSER_ERROR_PARSE_ERROR:
		fputs("parser error\n", out);
		break;
	case MCC_PARSER_ERROR_ALLOCATION_ERROR:
		fputs("allocation error\n", out);
		break;
	case MCC_PARSER_ERROR_UNABLE_TO_OPEN_STREAM:
		fputs("unable to open stream\n", out);
		break;
	default:
		assert(false);
		fputs("invalid error\n", out);
	}
}

struct mcc_parser_result mcc_parse_string(const char *input)
{
	assert(input);

	FILE *in = fmemopen((void *)input, strlen(input), "r");
	if (!in) {
		return (struct mcc_parser_result){
		    .error = MCC_PARSER_ERROR_UNABLE_TO_OPEN_STREAM,
		};
	}

	struct mcc_parser_result result = mcc_parse_file(in, NULL);

	fclose(in);

	return result;
}

struct mcc_parser_result mcc_parse_file(FILE *input, const char *filepath)
{
	assert(input);

	struct parser parser = {
	    .filepath = filepath,
	    .error = MCC_PARSER_ERROR_NONE,
	};

	mcc_lexer_init(&parser.lexer, input);

	// Prime first lexeme.
	parser_next(&parser);

	struct mcc_ast_expression *expr = parse_expression(&parser, 0);
	if (!expr) {
		parser_error_msg(&parser, MCC_PARSER_ERROR_PARSE_ERROR, parser.lexeme.sloc, "expected expression");
	}

	struct mcc_parser_result result = {
	    .expression = expr,
	    .error = parser.error,
	};
	snprintf(result.error_msg, sizeof(result.error_msg), "%s", parser.error_msg);

	mcc_lexer_deinit(&parser.lexer);

	return result;
}
