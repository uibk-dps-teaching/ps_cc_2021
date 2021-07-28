// Lexer
//
// This file defines the interface to the lexer (scanner) component of the
// compiler. Its task is to split a given character stream into easily
// digestable tokens for the parser. Numbers are automatically parsed to `long`
// or `double`.
//
// Ownership of the lexeme's `s_value` field is maintained by the lexer
// instance.

#ifndef MCC_LEXER_H
#define MCC_LEXER_H

#include <stdio.h>

#include "mcc/sloc.h"

// For simplicity we set an upper bound on the length of lexemes. Note that this
// does not cover comments as comments are discarded by the lexer anyway.
#define MCC_MAX_LEXEME_LENGTH 1024

enum mcc_token {
	MCC_TOKEN_IDENTIFIER,

	// Keywords:
	MCC_TOKEN_TRUE,
	MCC_TOKEN_FALSE,

	// Literals:
	MCC_TOKEN_INT_LITERAL,
	MCC_TOKEN_FLOAT_LITERAL,
	MCC_TOKEN_STRING_LITERAL,

	// Punctuation:
	MCC_TOKEN_PARENTH_LEFT,
	MCC_TOKEN_PARENTH_RIGHT,

	// Operators:
	MCC_TOKEN_PLUS,
	MCC_TOKEN_ASTERISK,
	MCC_TOKEN_SLASH,

	MCC_TOKEN_EOF,

	// Indicates an unhandled sequence of input characters discovered by the
	// lexer.
	MCC_TOKEN_UNKNOWN,

	// Indicates an error during the lexing phase. Observe the lexer's error
	// field for more information.
	MCC_TOKEN_ERROR,
};

const char *mcc_token_to_string(enum mcc_token token);

struct mcc_lexeme {
	enum mcc_token token;
	struct mcc_sloc sloc;

	union {
		// MCC_TOKEN_INT_LITERAL
		long i_value;

		// MCC_TOKEN_FLOAT_LITERAL
		double f_value;

		// MCC_TOKEN_IDENTIFIER
		// MCC_TOKEN_STRING_LITERAL
		// MCC_TOKEN_UNKNOWN
		char *s_value;
	};
};

void mcc_lexeme_print(FILE *out, const struct mcc_lexeme *lexeme);

enum mcc_lexer_error {
	MCC_LEXER_ERROR_NONE = 0,
	MCC_LEXER_ERROR_UNEXPECTED_EOF,
	MCC_LEXER_ERROR_STREAM_ERROR,
	MCC_LEXER_ERROR_ALLOCATION_ERROR,
	MCC_LEXER_ERROR_BUFFER_EXHAUSTION,
};

const char *mcc_lexer_error_to_string(enum mcc_lexer_error error);

struct mcc_lexer {
	FILE *stream;

	// This field holds the most recently extraced character from `stream`.
	char cur;
	struct mcc_sloc sloc;

	enum mcc_lexer_error error;

	// Internal buffer used for aggregating characters, commonly used to
	// populate the lexeme's `value` field.
	char buffer[MCC_MAX_LEXEME_LENGTH];
	size_t buffer_index;

	// This array of strings *owns* all identifiers, string literals, etc.
	// discovered during the lexing phase. The lexem's `s_value` field
	// references a string in this array if set.
	char **strings;
	size_t strings_count;
	size_t strings_capacity;
};

void mcc_lexer_init(struct mcc_lexer *lexer, FILE *stream);

void mcc_lexer_deinit(struct mcc_lexer *lexer);

// Call this function consecutively to obtain lexemes, one after another, until
// MCC_TOKEN_EOF is reached.
struct mcc_lexeme mcc_lexer_lex(struct mcc_lexer *lexer);

#endif // MCC_LEXER_H
