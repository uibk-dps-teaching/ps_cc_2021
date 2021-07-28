
int modulo(int n, int k) {
	return (n - k * (n / k));
}

bool is_palindrome(int original) {
	int reversed;
	int remainder;
	int to_check;

	reversed = 0;
	to_check = original;

	while (to_check != 0) {
		remainder = modulo(to_check, 10);
		reversed = reversed * 10 + remainder;
		to_check = to_check / 10;
	}

	if (original == reversed) {
		return true;
	} else {
		return false;
	}
}

int main() {
	int input;

	print("Please enter a number:");
	input = read_int();
	print_nl();

	bool result;
	result = is_palindrome(input);

	print_int(input);
	if (result) {
		print(" is a palindrome.");
	} else {
		print(" is not a palindrome.");
	}
	print_nl();

	return 0;
}
