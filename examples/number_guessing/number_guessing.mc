/* Test booleans and greater/smaller comparisons */

void guessNumber(int number, int max) {
    bool success;
    success = false;
    while(!success) {
        print("Enter a number between 0 and ");
        print_int(max);
        print_nl();
        int guess;
        guess = read_int();
        success = guess == number;
        if(success)
            print("you guessed correct");
        else {
            if(guess < number)
                print("the number is bigger");
            else
                print("the number is smaller");
            print_nl();
        }
    }
    print_nl();
}

int main() {
    int test;
    test = 3;
    guessNumber(test, 10);
    return 0;
}
