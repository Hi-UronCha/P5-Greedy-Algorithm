#ifndef SOLVERS_H
#define SOLVERS_H

#define MAX_N 100005

// 定义统一的返回结果
typedef struct {
    int count;      // 买的数量
    long long left; // 剩余金额
} Result;

// 声明两个求解函数
// 注意：参数完全一样，只有函数名不同
Result solve_v1(int N, long long D, int* prices, int* coupons);
Result solve_v2(int N, long long D, int* prices, int* coupons);

#endif