int euclid(int n, int k)
{
	if (k == 0) {
		return n;
	}

	if (n == 0) {
		return k;
	}

	if (n > k) {
		return euclid(n - k, k);
	} else {
		return euclid(n, k - n);
	}
}

int main()
{
	print("Please enter the first number: ");
	print_nl();

	int n;
	n = read_int();

	print("Please enter the second number: ");
	print_nl();

	int k;
	k = read_int();
	print_nl();

	int result;
	result = euclid(n, k);

	print("euclid(");
	print_int(n);
	print(", ");
	print_int(k);
	print(") = ");
	print_int(result);
	print_nl();

	return 0;
}
