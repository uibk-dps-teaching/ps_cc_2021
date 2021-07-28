#include "mcc/lexer.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

const char *mcc_token_to_string(enum mcc_token token)
{
	switch (token) {
	case MCC_TOKEN_IDENTIFIER:
		return "identifier";
	case MCC_TOKEN_TRUE:
		return "true";
	case MCC_TOKEN_FALSE:
		return "false";
	case MCC_TOKEN_INT_LITERAL:
		return "int literal";
	case MCC_TOKEN_FLOAT_LITERAL:
		return "float literal";
	case MCC_TOKEN_STRING_LITERAL:
		return "string literal";
	case MCC_TOKEN_PARENTH_LEFT:
		return "(";
	case MCC_TOKEN_PARENTH_RIGHT:
		return ")";
	case MCC_TOKEN_PLUS:
		return "+";
	case MCC_TOKEN_ASTERISK:
		return "*";
	case MCC_TOKEN_SLASH:
		return "/";
	case MCC_TOKEN_EOF:
		return "<EOF>";
	case MCC_TOKEN_UNKNOWN:
		return "<UNKNOWN_TOKEN>";
	case MCC_TOKEN_ERROR:
		return "<ERROR>";
	}

	assert(false);
	return "<ERROR_INVALID_TOKEN>";
}

void mcc_lexeme_print(FILE *out, const struct mcc_lexeme *lexeme)
{
	assert(out);
	assert(lexeme);

	switch (lexeme->token) {
	case MCC_TOKEN_IDENTIFIER:
		fprintf(out, "ident: %s", lexeme->s_value);
		break;
	case MCC_TOKEN_INT_LITERAL:
		fprintf(out, "%ld", lexeme->i_value);
		break;
	case MCC_TOKEN_FLOAT_LITERAL:
		fprintf(out, "%f", lexeme->f_value);
		break;
	case MCC_TOKEN_STRING_LITERAL:
		fprintf(out, "\"%s\"", lexeme->s_value);
		break;
	case MCC_TOKEN_UNKNOWN:
		fprintf(out, "%s (%s)", mcc_token_to_string(lexeme->token), lexeme->s_value);
		break;
	default:
		fprintf(out, "%s", mcc_token_to_string(lexeme->token));
	}
}

const char *mcc_lexer_error_to_string(enum mcc_lexer_error error)
{
	switch (error) {
	case MCC_LEXER_ERROR_NONE:
		return "none";
	case MCC_LEXER_ERROR_UNEXPECTED_EOF:
		return "unexpected EOF";
	case MCC_LEXER_ERROR_STREAM_ERROR:
		return "stream error";
	case MCC_LEXER_ERROR_ALLOCATION_ERROR:
		return "allocation error";
	case MCC_LEXER_ERROR_BUFFER_EXHAUSTION:
		return "buffer exhaustion";
	default:
		return "invalid lexer error";
	}
}

static int isalpha_(int c)
{
	return isalpha(c) || c == '_';
}

static int isalnum_(int c)
{
	return isalnum(c) || c == '_';
}

static void lexer_buffer_add(struct mcc_lexer *lexer, char c)
{
	assert(lexer);

	if (lexer->error) {
		return;
	}

	if (lexer->buffer_index >= sizeof(lexer->buffer)) {
		lexer->error = MCC_LEXER_ERROR_BUFFER_EXHAUSTION;
		return;
	}

	lexer->buffer[lexer->buffer_index++] = c;
}

static void lexer_buffer_reset(struct mcc_lexer *lexer)
{
	assert(lexer);
	lexer->buffer_index = 0;
}

static bool lexer_buffer_cmp(struct mcc_lexer *lexer, const char *s)
{
	assert(lexer);
	return strlen(s) == lexer->buffer_index && strncmp(s, lexer->buffer, lexer->buffer_index) == 0;
}

static void lexer_grow_strings(struct mcc_lexer *lexer)
{
	assert(lexer);

	if (lexer->error) {
		return;
	}

	size_t new_capacity = lexer->strings_capacity * 2;
	void *new_strings = realloc(lexer->strings, sizeof(lexer->strings[0]) * new_capacity);
	if (!new_strings) {
		lexer->error = MCC_LEXER_ERROR_ALLOCATION_ERROR;
		return;
	}

	lexer->strings = new_strings;
	lexer->strings_capacity = new_capacity;
}

static char *lexer_add_string(struct mcc_lexer *lexer, const char *s, size_t n)
{
	assert(lexer);

	if (lexer->error) {
		return NULL;
	}

	if (lexer->strings_count == lexer->strings_capacity) {
		lexer_grow_strings(lexer);
	}

	if (lexer->error) {
		return NULL;
	}

	char *new_string = strndup(s, n);
	if (!new_string) {
		lexer->error = MCC_LEXER_ERROR_ALLOCATION_ERROR;
		return NULL;
	}

	assert(lexer->strings_count < lexer->strings_capacity);
	lexer->strings[lexer->strings_count++] = new_string;

	return new_string;
}

// Grabs the next character from the input stream. Updates `sloc` accordingly.
// This function is typically called last by every lexer rule to setup the lexer
// for the next `mcc_lexer_lex` call.
static void lexer_next(struct mcc_lexer *lexer)
{
	assert(lexer);

	// advance location
	if (lexer->cur == '\n') {
		lexer->sloc.line++;
		lexer->sloc.column = 1;
	} else if (lexer->cur == '\t') {
		lexer->sloc.column += 8;
	} else if (lexer->cur != '\0') {
		lexer->sloc.column++;
	}

	if (fread(&lexer->cur, sizeof(char), 1, lexer->stream) != 1) {
		lexer->cur = '\0';
		if (ferror(lexer->stream)) {
			lexer->error = MCC_LEXER_ERROR_STREAM_ERROR;
		}
	}
}

// Returns true and advances the input iff the given character matches.
static bool lexer_accept(struct mcc_lexer *lexer, char c)
{
	assert(lexer);

	if (lexer->cur == c) {
		lexer_next(lexer);
		return true;
	}

	return false;
}

static void lexer_read_identifier_or_keywords(struct mcc_lexer *lexer, struct mcc_lexeme *lexeme)
{
	assert(lexer);
	assert(lexeme);

	if (lexer->error) {
		return;
	}

	lexeme->token = MCC_TOKEN_IDENTIFIER;

	for (; isalnum_(lexer->cur); lexer_next(lexer)) {
		lexer_buffer_add(lexer, lexer->cur);
	}

	// check for keywords
	if (lexer_buffer_cmp(lexer, "true")) {
		lexeme->token = MCC_TOKEN_TRUE;
	} else if (lexer_buffer_cmp(lexer, "false")) {
		lexeme->token = MCC_TOKEN_FALSE;
	} else {
		// not a keyword
		lexeme->s_value = lexer_add_string(lexer, lexer->buffer, lexer->buffer_index);
	}
}

static void lexer_read_number(struct mcc_lexer *lexer, struct mcc_lexeme *lexeme)
{
	assert(lexer);
	assert(lexeme);

	if (lexer->error) {
		return;
	}

	lexeme->token = MCC_TOKEN_INT_LITERAL;
	for (; isdigit(lexer->cur); lexer_next(lexer)) {
		lexer_buffer_add(lexer, lexer->cur);
	}

	if (lexer_accept(lexer, '.')) {
		lexeme->token = MCC_TOKEN_FLOAT_LITERAL;
		lexer_buffer_add(lexer, '.');

		// At least one digit after the period is required.
		if (!isdigit(lexer->cur)) {
			lexeme->token = MCC_TOKEN_UNKNOWN;
			return;
		}

		for (; isdigit(lexer->cur); lexer_next(lexer)) {
			lexer_buffer_add(lexer, lexer->cur);
		}
	}

	// Buffer needs to be null-terminated for `atol` / `atof`.
	lexer_buffer_add(lexer, '\0');
	if (lexer->error == MCC_LEXER_ERROR_BUFFER_EXHAUSTION) {
		return;
	}

	if (lexeme->token == MCC_TOKEN_INT_LITERAL) {
		lexeme->i_value = atol(lexer->buffer);
	} else if (lexeme->token == MCC_TOKEN_FLOAT_LITERAL) {
		lexeme->f_value = atof(lexer->buffer);
	}
}

static void lexer_read_string(struct mcc_lexer *lexer, struct mcc_lexeme *lexeme)
{
	assert(lexer);
	assert(lexeme);

	if (lexer->error) {
		return;
	}

	lexeme->token = MCC_TOKEN_STRING_LITERAL;

	for (; !feof(lexer->stream) && lexer->cur != '\"'; lexer_next(lexer)) {
		lexer_buffer_add(lexer, lexer->cur);
	}

	if (feof(lexer->stream)) {
		lexer->error = MCC_LEXER_ERROR_UNEXPECTED_EOF;
		return;
	}

	lexer_next(lexer); // skip closing double quote

	lexeme->s_value = lexer_add_string(lexer, lexer->buffer, lexer->buffer_index);
}

static void lexer_skip_comment(struct mcc_lexer *lexer)
{
	assert(lexer);

	for (; !feof(lexer->stream); lexer_next(lexer)) {
		if (lexer_accept(lexer, '*') && lexer_accept(lexer, '/')) {
			return;
		}
	}

	lexer->error = MCC_LEXER_ERROR_UNEXPECTED_EOF;
}

static void lexer_skip_ws(struct mcc_lexer *lexer)
{
	assert(lexer);

	for (; isspace(lexer->cur); lexer_next(lexer)) {
	}
}

void mcc_lexer_init(struct mcc_lexer *lexer, FILE *stream)
{
	assert(lexer);

	const size_t initial_strings_capacity = 8;

	*lexer = (struct mcc_lexer){
	    .stream = stream,
	    .sloc =
	        {
	            .line = 1,
	            .column = 1,
	        },
	    .strings = malloc(sizeof(lexer->strings[0]) * initial_strings_capacity),
	    .strings_capacity = initial_strings_capacity,
	};

	if (!lexer->strings) {
		lexer->error = MCC_LEXER_ERROR_ALLOCATION_ERROR;
		return;
	}

	// Prime `cur` for the first `mcc_lexer_lex` call.
	lexer_next(lexer);
}

void mcc_lexer_deinit(struct mcc_lexer *lexer)
{
	if (!lexer) {
		return;
	}

	for (size_t i = 0; i < lexer->strings_count; i++) {
		free(lexer->strings[i]);
	}

	free(lexer->strings);
}

struct mcc_lexeme mcc_lexer_lex(struct mcc_lexer *lexer)
{
	assert(lexer);

	while (true) {
		lexer_buffer_reset(lexer);

		lexer_skip_ws(lexer);

		struct mcc_lexeme result = {
		    .sloc = lexer->sloc,
		};

		// lexing rules
		{
			if (feof(lexer->stream)) {
				result.token = MCC_TOKEN_EOF;
			}

			else if (isalpha_(lexer->cur)) {
				lexer_read_identifier_or_keywords(lexer, &result);
			}

			else if (isdigit(lexer->cur)) {
				lexer_read_number(lexer, &result);
			}

			else if (lexer_accept(lexer, '\"')) {
				lexer_read_string(lexer, &result);
			}

			else if (lexer_accept(lexer, '(')) {
				result.token = MCC_TOKEN_PARENTH_LEFT;
			}

			else if (lexer_accept(lexer, ')')) {
				result.token = MCC_TOKEN_PARENTH_RIGHT;
			}

			else if (lexer_accept(lexer, '+')) {
				result.token = MCC_TOKEN_PLUS;
			}

			else if (lexer_accept(lexer, '*')) {
				result.token = MCC_TOKEN_ASTERISK;
			}

			else if (lexer_accept(lexer, '/')) {
				if (lexer_accept(lexer, '*')) {
					lexer_skip_comment(lexer);
					continue;
				} else {
					result.token = MCC_TOKEN_SLASH;
				}
			}

			else {
				result.token = MCC_TOKEN_UNKNOWN;
				result.s_value = lexer_add_string(lexer, &lexer->cur, 1);
				lexer_next(lexer);
			}
		}

		if (lexer->error) {
			result.token = MCC_TOKEN_ERROR;
			result.sloc = lexer->sloc;
		}

		return result;
	}
}
