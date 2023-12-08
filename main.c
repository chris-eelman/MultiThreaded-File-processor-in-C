#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    FILE *fptr;
    int thread_num;
} thread_args;

int compare(const void *a, const void *b) {
    int int_a = *((int*) a);
    int int_b = *((int*) b);

    if (int_a == int_b) return 0;
    else if (int_a < int_b) return -1;
    else return 1;
}



void* work(void* args) {
    thread_args* actual_args = (thread_args*) args;
    FILE *fptr = actual_args->fptr;
    int thread_num = actual_args->thread_num;


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
        fprintf(out_file, "\nSorted Numbers from  input%d.txt\n ", thread_num); // Write to the output file

        for (int i = 0; i < n; i++) {
            //printf("%d ", arr[i]); // print each sorted number
            fprintf(out_file, "%d\n", arr[i]); // Write each sorted number to the output file
        }




        free(arr); // free the memory allocated for arr
    } else {
        printf("File Open ERROR");
    }
    fclose(fptr);
    return 0;
}



int main() {
    // Open and clear the output file
    FILE *output = fopen("output.txt", "w");
    fprintf(output, "Sorted numbers: \n\n"); //


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
