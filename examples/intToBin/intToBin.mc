/*
 * Example input, written in mC
 *
 * This program coverts three non-negative integers (entered by the user) into their binary representation.
 * The result is printed to the console.
 */

void convert2bin(int number){
	/*Recursive conversion of an integer into binary*/

	if (number!=0){
		int quot;
		quot = number/2;

		if(quot>0){
			convert2bin(quot);
		}

		int remainder;
		remainder=number-2*quot;

		print_int(remainder);
	}
	else{
		print_int(0);
	}
}

int main()
{
	int[3] array;
	int counter;
	counter=1;

	/* Get user input*/
	while(counter<=3){
		print("Enter a non-negative integer (");
		print_int(counter);
		print(" of 3): ");

		array[counter-1]=read_int();
		counter=counter+1;

	}

	print_nl();
	/* Compute binary numbers and print them*/
	counter=0;

	while(counter<3){
		print("Binary representation of ");
		print_int(array[counter]);
		print(": ");

		convert2bin(array[counter]);
		print_nl();
		counter=counter+1;
	}

	return 0;
}
