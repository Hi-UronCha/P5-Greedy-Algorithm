#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solvers.h"
#include <time.h>

// 测试用例（默认输入已保证：所有优惠券 ≤ 所有商品）
typedef struct {
    const char* name;
    int N;
    long long D;
    int* prices;
    int* coupons;
    int expect_count;
    long long expect_left;
} Testcase;

// 运行单个测试用例（无任何约束检测）
static void run_test(Testcase* tc) {
    printf("=== Test: %s ===\n", tc->name);
    
    // 复制数组避免原数据被修改
    int* p = (int*)malloc(tc->N * sizeof(int));
    int* c = (int*)malloc(tc->N * sizeof(int));
    memcpy(p, tc->prices, tc->N * sizeof(int));
    memcpy(c, tc->coupons, tc->N * sizeof(int));

    // 执行测试
    clock_t start = clock();
    Result res = solve_v2(tc->N, tc->D, p, c);
    double time_ms = (double)(clock() - start) / CLOCKS_PER_SEC * 1000;

    // 输出结果
    printf("Prices: "); for (int i=0; i<tc->N; i++) printf("%d ", tc->prices[i]);
    printf("\nCoupons: "); for (int i=0; i<tc->N; i++) printf("%d ", tc->coupons[i]);
    printf("\nD=%lld | prediction: count=%d, left=%lld | actual: count=%d, left=%lld\n",
           tc->D, tc->expect_count, tc->expect_left, res.count, res.left);
    printf(res.count == tc->expect_count && res.left == tc->expect_left ? "Pass\n" : "Fail\n");
    printf("Time: %.2f ms\n--------------------------------------------------\n\n", time_ms);

    free(p);
    free(c);
}

// 比较函数：用于排序2N个随机数
static int cmp_long(const void* a, const void* b) {
    return *(long long*)a - *(long long*)b;
}

// 单次大数据测试（内部使用，不单独输出详细信息）
static int single_large_test(int N, long long D, int test_idx) {
    // 动态分配内存
    long long* all_nums = (long long*)malloc(2 * N * sizeof(long long));
    int* prices = (int*)malloc(N * sizeof(int));
    int* coupons = (int*)malloc(N * sizeof(int));
    if (!all_nums || !prices || !coupons) {
        // 内存分配失败，返回-1
        if (all_nums) free(all_nums);
        if (prices) free(prices);
        if (coupons) free(coupons);
        return -1;
    }

    // 生成随机数（种子=12345+test_idx，确保每次测试数据不同但可复现）
    srand(12345 + test_idx);
    for (int i = 0; i < 2 * N; i++) {
        long long high = (long long)(rand()) << 15;
        long long low = rand();
        all_nums[i] = (high | low) % 1000000000 + 1;  // 1~1e9
    }

    // 排序+拆分商品/优惠券
    qsort(all_nums, 2 * N, sizeof(long long), cmp_long);
    for (int i = 0; i < N; i++) {
        coupons[i] = (int)all_nums[i];
        prices[i] = (int)all_nums[N + i];
    }

    // 执行测试（仅计时，不输出详细结果）
    clock_t start = clock();
    Result res = solve_v2(N, D, prices, coupons);
    (void)res;  // 忽略结果（重点统计时间，符合题目要求）

    // 释放内存
    free(all_nums);
    free(prices);
    free(coupons);

    // 返回单次耗时（毫秒）
    return (int)((double)(clock() - start) / CLOCKS_PER_SEC * 1000);
}

// 多次重复大数据测试（核心新增功能）
static void test_large_scale_repeat(int repeat_cnt, int N, long long D, const char* desc) {
    printf("=== Large Scale Repeat Test: %s ===\n", desc);
    printf("Configuration: Repeat=%d times, N=%d, D=%lld\n", repeat_cnt, N, D);

    clock_t total_start = clock();  // 总计时开始
    long long total_time_ms = 0;    // 总耗时（毫秒）
    int success_cnt = 0;            // 成功测试次数
    int fail_cnt = 0;               // 内存分配失败次数

    // 循环执行repeat_cnt次测试
    for (int i = 0; i < repeat_cnt; i++) {
        // 进度提示（每100次输出一次，避免刷屏）
        if ((i + 1) % 100 == 0) {
            printf("Progress: %d/%d tests completed\n", i + 1, repeat_cnt);
        }

        int single_time = single_large_test(N, D, i);
        if (single_time == -1) {
            fail_cnt++;
        } else {
            total_time_ms += single_time;
            success_cnt++;
        }
    }

    // 总计时结束，计算统计结果
    clock_t total_end = clock();
    double total_time_s = (double)(total_end - total_start) / CLOCKS_PER_SEC;
    double avg_time_ms = success_cnt > 0 ? (double)total_time_ms / success_cnt : 0;

    // 输出统计报告
    printf("\n==================== Repeat Test Statistics ====================\n");
    printf("Total Tests: %d\n", repeat_cnt);
    printf("Success Tests: %d\n", success_cnt);
    printf("Memory Allocation Failures: %d\n", fail_cnt);
    printf("Total Running Time: %.2f s\n", total_time_s);
    printf("Average Time per Success Test: %.2f ms\n", avg_time_ms);
    printf("===============================================================\n\n");
}

int main() {

    // 1. 运行核心小用例（保持不变）
    Testcase cases[] = {
        {
            .name = "purchases with and without coupon",
            .N = 1, .D = 20,
            .prices = (int[]){10}, .coupons = (int[]){8},
            .expect_count = 2, .expect_left = 8
        },
        {
            .name = "sample test case",
            .N = 4, .D = 30,
            .prices = (int[]){12, 10,15,20}, .coupons = (int[]){9,6,8,7},
            .expect_count = 8, .expect_left = 2
        },
        {
            .name = "normal case",
            .N = 2, .D = 10,
            .prices = (int[]){12, 10}, .coupons = (int[]){7, 5},
            .expect_count = 2, .expect_left = 2
        },
        {
            .name = "buy the same item multiple times without coupon",
            .N = 3, .D = 100,
            .prices = (int[]){20, 18, 15}, .coupons = (int[]){12, 10, 8},
            .expect_count = 11, .expect_left = 1
        },
        {
            .name = "money left is not enough to buy any item",
            .N = 3, .D = 1,
            .prices = (int[]){20, 18, 15}, .coupons = (int[]){12, 10, 8},
            .expect_count = 0, .expect_left = 1
        }
    };

    int case_cnt = sizeof(cases)/sizeof(cases[0]);
    for (int i=0; i<case_cnt; i++) run_test(&cases[i]);
    
    // 2. 运行多次重复大数据测试（可按需调整重复次数、N、D）
    // 示例1：N=100000, D=1e4 → 重复1000次
    test_large_scale_repeat(1000, 100000, 10000, "N=100000, D=1e4 (Repeat 1000 Times)");
    // 示例2：N=100000, D=1e5 → 重复1000次（平衡时间和统计意义）
    test_large_scale_repeat(1000, 100000, 100000, "N=100000, D=1e5 (Repeat 1000 Times)");
    // 示例3：N=100000, D=1e6 → 重复1000次
    test_large_scale_repeat(1000, 100000, 1000000, "N=1e5, D=1e6 (Repeat 1000 Times)");
    //以上是N固定时随D变化的测试
    // 示例4：N=1000, D=1e6 → 重复1000次
    test_large_scale_repeat(1000, 1000, 1000000, "N=1000, D=1e6 (Repeat 1000 Times)");
    //示例5：N=10000, D=1e6 → 重复1000次
    test_large_scale_repeat(1000, 10000, 1000000, "N=10000, D=1e6 (Repeat 1000 Times)");
    //示例6：N=100000, D=1e6 → 重复1000次
    test_large_scale_repeat(1000, 100000, 1000000, "N=100000, D=1e6 (Repeat 1000 Times)");
    //以上是D固定时随N变化的测试
    return 0;
}
