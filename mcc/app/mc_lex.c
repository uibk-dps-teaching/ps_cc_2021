#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "mcc/lexer.h"

int main(void)
{
	struct mcc_lexer lexer;
	mcc_lexer_init(&lexer, stdin);

	while (true) {
		struct mcc_lexeme lexeme = mcc_lexer_lex(&lexer);
		if (lexeme.token == MCC_TOKEN_ERROR) {
			fprintf(stderr, "Lexer Error: %s\n", mcc_lexer_error_to_string(lexer.error));
			break;
		}

		mcc_sloc_print(stdout, lexeme.sloc);
		printf("\t");
		mcc_lexeme_print(stdout, &lexeme);
		printf("\n");

		if (lexeme.token == MCC_TOKEN_EOF) {
			break;
		}
	}

	mcc_lexer_deinit(&lexer);

	return EXIT_SUCCESS;
}
