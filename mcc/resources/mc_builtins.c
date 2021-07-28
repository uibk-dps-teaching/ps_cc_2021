#include <stdio.h>

void __attribute__((cdecl)) print(const char *msg);
void __attribute__((cdecl)) print_nl(void);
void __attribute__((cdecl)) print_int(int x);
void __attribute__((cdecl)) print_float(float x);
int __attribute__((cdecl)) read_int(void);
float __attribute__((cdecl)) read_float(void);

void print(const char *msg)
{
	printf("%s", msg);
}

void print_nl(void)
{
	printf("\n");
}

void print_int(int x)
{
	printf("%d", x);
}

void print_float(float x)
{
	printf("%.2f", x);
}

int read_int(void)
{
	int ret = 0;
	scanf("%d", &ret);
	return ret;
}

float read_float(void)
{
	float ret = 0.0f;
	scanf("%f", &ret);
	return ret;
}
