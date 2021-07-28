/* Divide integer "divisor" by integer "dividend" and output the remainder */

int calculate_remainder(int divisor, int dividend) {
  while (dividend > divisor) {
    dividend = dividend - divisor;
  }
  return dividend;
}

/* The divisor must not be smaller than or equal to 0 */

int main() {

  print("Enter Dividend:");
  print_nl();
  int dividend;
  dividend = read_int();
  print("Enter Divisor:");
  print_nl();
  int divisor;
  divisor = read_int();

  if (dividend < divisor) {
    print("Dividend has to be bigger than divisor.");
    print_nl();

  } else {

    int remainder;
    remainder = calculate_remainder(divisor, dividend);
    int result;
    result = (dividend - remainder) / divisor;
    print_int(dividend);
    print(" is divisible by ");
    print_int(divisor);
    print(" with the remainder ");
    print_int(remainder);
    print(" and result ");
    print_int(result);
    print_nl();
  }

  return 0;
}
