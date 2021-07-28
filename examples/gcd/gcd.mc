/* calulcate modulo */
int mod(int a, int b){
  return a-(a/b*b);
}

/* calulate greatest common divisor */
int gcd(int x, int y){
    int c;
    if (x < 0){
      x = -x;
    }

    if (y < 0){
      y = -y;
    }

    while (y != 0) {
      c = mod(x, y);
      x = y;
      y = c;
    }
    return x;
}

int main() {
    int a;
    int b;

    print("Please enter the first number: ");
    print_nl();
    a = read_int();
    print("Please enter the second number: ");
    print_nl();
    b = read_int();

    int res;
    res = gcd(a,b);

    print("The greatest common divisor is: ");
    print_int(res);
    print_nl();

    return 0;
}
