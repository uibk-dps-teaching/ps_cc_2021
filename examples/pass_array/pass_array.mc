void foo(int[10] array)
{
	array[5] = 42;
}

int main()
{
	int[10] array;
	array[5] = 23;

	foo(array);

	print_int(array[5]);
	print_nl();

	return 0;
}
