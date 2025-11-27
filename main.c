#include <stdio.h>
#include "solvers.h"
#include <stdlib.h>

int main(){
    int N;
    long long D;
    if (scanf("%d%lld", &N, &D) != 2) return 0;

    int *prices = (int *)malloc(N * sizeof(int));
    int *coupons = (int *)malloc(N * sizeof(int));
    
    for (int i = 0; i < N; i++) scanf("%d", &prices[i]);
    for (int i = 0; i < N; i++) scanf("%d", &coupons[i]);

    Result res = solve_v1(N, D, prices, coupons);

    printf("%d %lld\n", res.count, res.left);

    free(prices);
    free(coupons);

    return 0;
    
}
