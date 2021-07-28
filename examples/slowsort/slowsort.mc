/* implementation of slowsort on integers, based on https://en.wikipedia.org/wiki/Slowsort */
void i_slowsort(int[10] arr, int i, int j){
    if (i >= j){
        return;
    }
    int m;
    m = (i+j) / 2;
    i_slowsort(arr, i, m);
    i_slowsort(arr, m+1, j);
    if (arr[j] < arr[m]){ /* swap elements on index j and m */
        int tmp;
        tmp = arr[j];
        arr[j] = arr[m];
        arr[m] = tmp;
    }
    j = j - 1;
    i_slowsort(arr, i, j);
    return;
}

/* implementation of slowsort on floats, based on https://en.wikipedia.org/wiki/Slowsort */
void f_slowsort(float[10] arr, int i, int j){
    if (i >= j){
        return;
    }
    int m;
    m = (i+j) / 2;
    f_slowsort(arr, i, m);
    f_slowsort(arr, m+1, j);
    if (arr[j] < arr[m]){ /* swap elements on index j and m */
        float tmp;
        tmp = arr[j];
        arr[j] = arr[m];
        arr[m] = tmp;
    }
    j = j - 1;
    f_slowsort(arr, i, j);
    return;
}

/* prints int array to stdout */
void print_i_array(int[10] arr, int arr_size){
    int i;
    i = 0;
    while(i < arr_size){
        print_int(arr[i]);
        print(" ");
        i = i+1;
    }
    print_nl();
}

/* prints float array to stdout */
void print_f_array(float[10] arr, int arr_size){
    int i;
    i = 0;
    while(i < arr_size){
        print_float(arr[i]);
        print(" ");
        i = i+1;
    }
    print_nl();
}

int get_choice(){
    print("Do you want to sort: ");
    print_nl();
    print("[1] integers, or");
    print_nl();
    print("[2] floats?");
    print_nl();
    print("choice: ");
    print_nl();
    int choice;
    choice = read_int();
    return choice;
}

int main(){
    print("*** Implementation of Slowsort ***");
    print_nl();

    /* get user choice */
    int choice;
    choice = get_choice();

    bool is_int;
    if (choice == 1){
        is_int = true;
    }else if (choice == 2){
        is_int = false;
    }else{
        print("Invalid choice! Choice have to be either 1, or 2.");
        print_nl();
        return 0;
    }
    print("give a sequence of 10 numbers to be sorted: ");
    print_nl();

    int arr_size;
    arr_size = 10;
    int i;
    i=0;

    if (is_int){
        int[10] i_arr;
        int i_number;
        while(i < arr_size){
            print_int(i);
            print(". number = ");
            print_nl();
            i_number = read_int();
            i_arr[i] = i_number;
            i = i+1;
        }

        /* print input array to stdout */
        print("input array: ");
        print_i_array(i_arr, arr_size);

        /* call sorting function */
        i_slowsort(i_arr, 0, arr_size-1);

        /* print result array to stdout */
        print("output array: ");
        print_i_array(i_arr, arr_size);
    }else{
        float[10] f_arr;
        float f_number;

        while(i < arr_size){
            print_int(i);
            print(". number = ");
            f_number = read_float();
            f_arr[i] = f_number;
            i = i+1;
        }

        /* print input array to stdout */
        print("input array: ");
        print_f_array(f_arr, arr_size);

        /* call sorting function */
        f_slowsort(f_arr, 0, arr_size-1);

        /* print result array to stdout */
        print("output array: ");
        print_f_array(f_arr, arr_size);
    }

    return 0;
}
