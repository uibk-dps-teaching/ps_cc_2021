#ifndef MCC_SLOC_H
#define MCC_SLOC_H

#include <stdio.h>

struct mcc_sloc {
	int line;
	int column;
};

static inline void mcc_sloc_print(FILE *out, struct mcc_sloc sloc)
{
	fprintf(out, "%d:%d", sloc.line, sloc.column);
}

#endif // MCC_SLOC_H
