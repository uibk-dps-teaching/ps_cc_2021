#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcc/ast.h"
#include "mcc/parser.h"

void print_usage(const char *prg)
{
	printf("usage: %s <FILE>\n\n", prg);
	printf("  <FILE>        Input filepath or - for stdin\n");
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	// determine input source
	FILE *in;
	if (strcmp("-", argv[1]) == 0) {
		in = stdin;
	} else {
		in = fopen(argv[1], "r");
		if (!in) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	}

	struct mcc_ast_expression *expr = NULL;

	// parsing phase
	{
		struct mcc_parser_result result = mcc_parse_file(in, in == stdin ? "<stdin>" : NULL);
		fclose(in);
		if (result.error) {
			mcc_parser_result_print_error(stderr, &result);
			return EXIT_FAILURE;
		}
		expr = result.expression;
	}

	// TODO:
	// - run semantic checks
	// - create three-address code
	// - output assembly code
	// - invoke backend compiler

	// cleanup
	mcc_ast_delete_expression(expr);

	return EXIT_SUCCESS;
}
