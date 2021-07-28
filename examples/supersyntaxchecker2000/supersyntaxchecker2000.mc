/* This is a comment */
int main(){
	print("Please enter a number: ");
	print_nl();
	int n;
	n = read_int();
	bool a;
	a = 1 <= n;
	bool b;
	b = 2 == n;
	b = 3 >= n;
	n = n * 2;
	n = n / 2;
	n = n + 2;
	n = n - 2;
	a && b;
	a || b;
	a != !b;
	int[3] test;
	string hi;
	hi = "Hello";
	float f;
	f = -1.0;
	bool c;
	c = true;
	if (c)
	{
		while(c){
			c = false;
		}
	}
	if (c){
		f = f * 2.0;
	}
	else{
		f = f * 3.0;
	}
	return 0;
}
