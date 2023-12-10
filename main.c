//Chris Eelman
//Operating Systems final project pt.2


/*
This program is a multithreaded application in C that performs various file operations concurrently using pthreads.
 The main objective of the program is to read input from multiple files, sort the data in ascending order, and write
 the sorted results to a single output file.

The program works by creating a separate thread for each input file. Each thread is responsible for opening its assigned
 input file, reading the numbers from the file into an array, sorting the array using the qsort function which uses the compare function,
 and writing the sorted numbers to the output file. The program uses the pthread_create function to create a new thread for each input file, and the
 pthread_join function to wait for all threads to finish their execution. To ensure thread safety when accessing the
 output file, the program opens the file in append mode in each thread, writes the sorted numbers, and then closes the file.

One of the challenges faced in this program was to pass multiple arguments to the threads. This was overcome by defining
 a structure that contains both arguments, the file pointer and thread number, and passing a pointer to an instance of this structure to the
 pthread_create function. Another challenge was to ensure that the numbers are read correctly from the input files and
 stored in the array. This was achieved by using the fgets and sscanf functions to read each line from the file and convert it to an integer.

In terms of synchronization, since each thread is just APPENDING to the output file and not sharing any
 common resources with other threads, there is no need for things like mutexes in this program. This is why it is indicated in
 the output file which list is from which file because sometimes the output gets out of order due to a race condition

*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


typedef struct { // creates a structure, so I can use two arguments inside the thread, in this case what file to open and
    //the thread number, so I can indicate where the output is coming from.
    FILE *fptr;
    int thread_num;
} thread_args;

int compare(const void *a, const void *b) { // compare function used in qsort to sort in ascending order
    int int_a = *((int*) a);
    int int_b = *((int*) b);

    if (int_a == int_b) return 0;
    else if (int_a < int_b) return -1;
    else return 1;
}



void* work(void* args) {
    clock_t start_time = clock(); // start time for each thread

    thread_args* actual_args = (thread_args*) args; // stores the arguments from the thread, this way I can use 2 arguments
    FILE *fptr = actual_args->fptr; // stores the file from the arguments
    int thread_num = actual_args->thread_num; // stores the thread number


    FILE *out_file = fopen("output.txt", "a"); // Open the output file

    char buffer[1000];
    int *arr; // declare an array to store the numbers
    int n = 0; // declare a variable to store the number of elements

    if (fptr != NULL) {
        while (fgets(buffer, 1000, fptr)) {
            //printf("Line: %s", buffer);
            arr = realloc(arr, (n + 1) * sizeof(int)); // allocate or reallocate memory for the array
            if (arr == NULL) {
                printf("Memory allocation error\n");
                return NULL;
            }
            sscanf(buffer, "%d", &arr[n]); // read each number from the buffer and store it in the array
            n++;
        }
        qsort(arr, n, sizeof(int), compare); // sort the array using qsort and compare function
        fprintf(out_file, "\nSorted Numbers from input%d.txt\n ", thread_num); // Write to the output file

        for (int i = 0; i < n; i++) {
            //printf("%d ", arr[i]);
            fprintf(out_file, "%d\n", arr[i]); // Write each sorted number to the output file
        }
        free(arr); // free the memory allocated for arr
        printf("Thread%d Successfully wrote to output.txt \n", thread_num);


    } else {
        printf("File Open ERROR");
        return NULL;
    }
    fclose(fptr);

    clock_t end_time = clock();
    double execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Thread %d execution time: %.7f seconds\n", thread_num, execution_time);

    return 0;
}



int main() {
    // Opens and clears the output file if anything was there before.
    FILE *output = fopen("output.txt", "w");
    fprintf(output, "Sorted numbers from all the input files \n\n"); //


    pthread_t t1, t2, t3;
    FILE *fptr1, *fptr2, *fptr3;
    thread_args args1 = {fopen("input1.txt", "r"), 1};
    thread_args args2 = {fopen("input2.txt", "r"), 2};
    thread_args args3 = {fopen("input3.txt", "r"), 3};


    pthread_create(&t1, NULL, work, &args1);
    pthread_create(&t2, NULL, work, &args2);
    pthread_create(&t3, NULL, work, &args3);


    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}
