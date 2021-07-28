#include <stdio.h>
#include <stdlib.h>

#include <CuTest.h>

int main(void)
{
	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

#define TEST(test) SUITE_ADD_TEST(suite, test);
	TESTS
#undef TEST

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);

	int ret = suite->failCount == 0 ? EXIT_SUCCESS : EXIT_FAILURE;

	CuStringDelete(output);
	CuSuiteDelete(suite);

	return ret;
}
