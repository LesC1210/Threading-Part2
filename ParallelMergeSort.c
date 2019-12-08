#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIZE 5

int array[SIZE];

void fillArrayWithRandomNumbers(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) array[i] = rand()%100;
}

void printArray(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) printf("%5d", arr[i]);
    printf("\n");
}

typedef struct StartEndIndexes {
    int start;
    int end;
} StartEndIndexes;

// Runs mergesort on the array segment described in the
// argument. Spawns two threads to mergesort each half
// of the array segment, and then merges the results.

void merge(int left, int mid, int right){
    int k;
    int upper = mid - left + 1; //upper half of the array size 
    int lower = right - mid; //lower half of the array size
    int leftHalf[upper];
    int rightHalf[lower];
    
    for(int i = 0; i < upper; i++){
        leftHalf[i] = array[left + i];
    }
    for(int j = 0; j < lower; j++){
        rightHalf[j] = array[mid + j + 1];
    }
    
    int i = 0;
    int j = 0;
    k = left;
    
    while(i < upper && j < lower){
        if(leftHalf[i] <= rightHalf[j]){
            array[k] = leftHalf[i];
            i++;
        }
        else{
            array[k] = rightHalf[j];
            j++;
        }
        k++;
    }
    
    while(i < upper){
        array[k] = leftHalf[i];
        i++;
        k++;
    }
    
    while(j < lower){
        array[k] = rightHalf[j];
        j++;
        k++;
    }
}


void* mergeSort(void* args) {
    StartEndIndexes indexes = *((StartEndIndexes*)args);
    int error1;
    int error2;
    int midpoint;

    if (indexes.start < indexes.end){ 
        midpoint = (indexes.start + indexes.end) / 2; 
        StartEndIndexes indexes1, indexes2;
        indexes1.start = indexes.start;
        indexes1.end = midpoint;
        indexes2.start = midpoint + 1;
        indexes2.end = indexes.end;
        pthread_t left_tid;
        pthread_t right_tid;
        
        error1 = pthread_create(&left_tid, NULL, mergeSort, &indexes1);
        error2 = pthread_create(&right_tid, NULL, mergeSort, &indexes2);
        
        if (error1 != 0){
            printf("Error sorting left half.\n");
        }
        if (error2 != 0){
            printf("Error sorting right half.\n");
        }
        
        error1 = pthread_join(left_tid, NULL);
        error2 = pthread_join(right_tid, NULL);
        
        if (error1 != 0){
            printf("Error sorting left half.\n");
        }
        if (error2 != 0){
            printf("Error sorting right half.\n");
        }
        merge(indexes.start, midpoint, indexes.end);
    } 
    return NULL;
}

int main() {
    srand(time(0));
    StartEndIndexes indexes;
    indexes.start = 0;
    indexes.end = SIZE - 1;
    int error;
    
    // 1. Fill array with random numbers.
    fillArrayWithRandomNumbers(array);
    
    // 2. Print the array.
    printf("Unsorted array: ");
    printArray(array);
    
    // 3. Create a thread for merge sort.
    pthread_t tid;
    error = pthread_create(&(tid), NULL, &mergeSort, &indexes);
    if (error != 0){
        printf("Error creating thread.\n");
    }
    
    // 4. Wait for mergesort to finish.
    error = pthread_join(tid, NULL);
    if (error != 0){
        printf("Error joining thread.\n");
    }
    
    // 5. Print the sorted array.
    printf("Sorted array:   ");
    printArray(array);
}