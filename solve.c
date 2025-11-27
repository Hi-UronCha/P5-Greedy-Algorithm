#include "solvers.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    int item_idx;
    int coupon_idx;
    long long cost;
} Node;

Node heap[MAX_N];
int heap_size = 0;

const int compareAsc(const void *a, const void *b){
    return (*(int *)a - *(int *)b);
}

const int compareDesc(const void *a, const void *b){
    return (*(int *)b - *(int *)a);
}

const void insert(Node item){
    if (heap_size >= MAX_N){
        printf("heap is fullfilled\n");
        return;
    }
    heap[heap_size] = item;
    int index = heap_size;
    while (index > 0){
        int parent = (index - 1) / 2;
        if (heap[index].cost < heap[parent].cost){
            Node temp = heap[index];
            heap[index] = heap[parent];
            heap[parent] = temp;
            index = parent;
        } else {
            break;
        }
    }

    heap_size++;
}

const void siftDown(int index){
    while (2*index + 1 < heap_size){
        int child = 2*index + 1;

        if (child + 1 < heap_size && heap[child].cost > heap[child + 1].cost)
            child++;

        if (heap[child].cost < heap[index].cost){
            Node temp = heap[child];
            heap[child] = heap[index];
            heap[index] = temp;

            index = child;
        } else {
            break;
        }
    }
}

Node deleteMin(){
    if (heap_size == 0){
        printf("empty heap!\n");
        
        exit(1);
    }

    Node min = heap[0];
    heap[0] = heap[heap_size-1];
    heap_size--;
    siftDown(0);
    return min;
}

Result solve_v1(int N, long long D, int* prices, int* coupons){
    heap_size = 0;

    qsort(prices, N, sizeof(int), compareAsc);
    qsort(coupons, N, sizeof(int), compareDesc);

    for(int i = 0; i < N; i++){
        Node temp;
        temp.item_idx = i;
        temp.coupon_idx = 0;
        temp.cost = (long long)(prices[i] - coupons[0]);

        insert(temp);
    }

    int count = 0;
    while (heap_size > 0){
        Node current = deleteMin();

        if (D >= current.cost){
            D -= current.cost;
            count++;
        } else {
            break;
        }

        int next_coupon_idx = current.coupon_idx + 1;
        if (next_coupon_idx < N){
            Node next;
            next.item_idx = current.item_idx;
            next.coupon_idx = next_coupon_idx;
            next.cost = (long long)(prices[next.item_idx] - coupons[next.coupon_idx]);

            insert(next);
        }
    }

    Result res = {count, D};

    return res;
}