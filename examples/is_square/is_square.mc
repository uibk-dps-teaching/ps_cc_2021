/* is_squared returns true if a number is the square of any integer */

bool is_square(int n)
{
	int i;
	i = 0;
	while(i*i <= n){
		if(i*i == n){
			return true;
		}
	i = i + 1;
	}
	return false;
}

int main()
{
	print("Please enter a number: ");
	print_nl();
	int n;
	n = read_int();
	if(is_square(n)){
		print("Yes");
	}
	else
	{
		print("No");
	}
	print_nl();
	return 0;
}
