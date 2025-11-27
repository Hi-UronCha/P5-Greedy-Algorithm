#include <stdio.h>
#include <stdlib.h>
#include "solvers.h"

long long heap_cost[MAX_N];
int heap_p_idx[MAX_N];
int heap_c_idx[MAX_N];
int size = 0;

#define SWAP(i, j) {\
    long long temp_cost = heap_cost[i]; heap_cost[i] = heap_cost[j]; heap_cost[j] = temp_cost;\
    int temp_p_idx = heap_p_idx[i]; heap_p_idx[i] = heap_p_idx[j]; heap_p_idx[j] = temp_p_idx;\
    int temp_c_idx = heap_c_idx[i]; heap_c_idx[i] = heap_c_idx[j]; heap_c_idx[j] = temp_c_idx;\
}

// compare functions for qsort

const int compareAsc(const void *a, const void *b){
    return (*(int *)a - *(int *)b);
}

const int compareDesc(const void *a, const void *b){
    return (*(int *)b - *(int *)a);
}

// the implementation for 4-ary Heap

void insert(int p_idx, int c_idx, long long cost){
    if (size > MAX_N){
        printf("heap is fullfilled\n");
        return;
    }

    heap_cost[size] = cost;
    heap_c_idx[size] = c_idx;
    heap_p_idx[size] = p_idx;

    int index = size;

    while (index > 0){
        int parent = (index - 1) / 4;

        if (heap_cost[index] < heap_cost[parent]){
            SWAP(index, parent);
            index = parent;
        } else {
            break;
        }
    }

    size++;
}

const void shiftDown(int index){
    while (4*index + 1 < size){
        int child = 4*index + 1;

        int min_child = child;

        if (child + 1 < size && heap_cost[child + 1] < heap_cost[min_child]) min_child = child + 1;
        if (child + 2 < size && heap_cost[child + 2] < heap_cost[min_child]) min_child = child + 2;
        if (child + 3 < size && heap_cost[child + 3] < heap_cost[min_child]) min_child = child + 3;

        if (heap_cost[index] > heap_cost[min_child]) {
            SWAP(index, min_child);
            index = min_child;
        } else {
            break;
        }
    }
}

const void deleteMin(){
    if (size == 0){
        printf("empty heap!\n");
        exit(1);
    }

    heap_cost[0] = heap_cost[size - 1];
    heap_p_idx[0] = heap_p_idx[size - 1];
    heap_c_idx[0] = heap_c_idx[size - 1];

    size--;
    shiftDown(0);

    return;
}

Result solve_v2(int N, long long D, int* prices, int* coupons){
    size = 0;

    qsort(prices, N, sizeof(int), compareAsc);
    qsort(coupons, N, sizeof(int), compareDesc);

    insert(0, 0, (long long)(prices[0] - coupons[0]));

    int count = 0;

    while (size > 0){
        long long current_cost = heap_cost[0];
        int current_p_idx = heap_p_idx[0];
        int current_c_idx = heap_c_idx[0];

        deleteMin();

        if (D >= current_cost){
            D -= current_cost;
            count++;
        } else {
            break;
        }

        int next_c_idx = current_c_idx + 1;
        if (next_c_idx < N){
            long long next_cost = (long long)(prices[current_p_idx] - coupons[next_c_idx]);
            if (next_cost <= D){
                insert(current_p_idx, next_c_idx, next_cost);
            }
        }


        if (current_c_idx == 0){
            int next_p_idx = current_p_idx + 1;
            if (next_p_idx < N){
                long long next_cost = (long long)(prices[next_p_idx] - coupons[0]);
                if (next_cost <= D){
                    insert(next_p_idx, 0, next_cost);
                }
            }

        }
    }

    Result res = {count, D};

    return res;
}