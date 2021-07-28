int calculateFactorial(int number){
    int factorial;
    factorial = 1;
    int i;
    i = 1;
    while (i <= number){
        factorial = factorial * i;
        i = i + 1;
    }
    return factorial;
}

int main() {

    print("Please enter a number: ");

    int number;
    number = read_int();
    print_nl();

    int result;
    result = calculateFactorial(number);

    print("The factorial of ");
    print_int(number);
    print(" is ");
    print_int(result);
    print_nl();

    return 0;
}
