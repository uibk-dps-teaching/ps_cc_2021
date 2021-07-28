int main()
{
	int x;
	x = 1;
	print_int(x);

	{
		x = 2;
		print_int(x);

		int x;
		x = 3;
		print_int(x);

		{
			x = 4;
			print_int(x);

			int x;
			x = 5;
			print_int(x);
		}

		print_int(x);
	}

	print_int(x);
	print_nl();

	return 0;
}
