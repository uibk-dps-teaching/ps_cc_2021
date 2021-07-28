// Parser Infrastructure
//
// This defines the interface to the parser component of the compiler.
//
// The parser tries to convert a given text input to an AST. On success,
// ownership of the AST is transferred to the caller via the `mcc_parser_result`
// struct.

#ifndef MCC_PARSER_H
#define MCC_PARSER_H

#include <stdio.h>

#include "mcc/ast.h"

enum mcc_parser_error {
	MCC_PARSER_ERROR_NONE = 0,
	MCC_PARSER_ERROR_LEXER_ERROR,
	MCC_PARSER_ERROR_PARSE_ERROR,
	MCC_PARSER_ERROR_ALLOCATION_ERROR,
	MCC_PARSER_ERROR_UNABLE_TO_OPEN_STREAM,
};

struct mcc_parser_result {
	struct mcc_ast_expression *expression;

	enum mcc_parser_error error;
	char error_msg[1024];
};

void mcc_parser_result_print_error(FILE *out, struct mcc_parser_result *result);

struct mcc_parser_result mcc_parse_string(const char *input);

// Runs the parser on the given `input`. `filepath` is only used for prefixing
// error messages and can be NULL.
struct mcc_parser_result mcc_parse_file(FILE *input, const char *filepath);

#endif // MCC_PARSER_H
