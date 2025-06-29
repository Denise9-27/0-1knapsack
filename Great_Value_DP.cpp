#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#define MAX_N 320000  // 最大物品数量
typedef struct {
    int weight;
    double value;  
} Item;

Item items[MAX_N];
int n;
int capacity;
double max_value; 

// 动态规划法求解（针对大规模数据优化）
void dynamicProgrammingLarge() {
    int *dp = (int*)malloc((capacity + 1) * sizeof(int));
    memset(dp, 0, (capacity + 1) * sizeof(int));

    for (int i = 0; i < n; i++) {
        for (int w = capacity; w >= items[i].weight; w--) {
            if (dp[w - items[i].weight] + items[i].value > dp[w]) {
                dp[w] = dp[w - items[i].weight] + items[i].value;
            }
        }
    }

    max_value = dp[capacity];
    free(dp);
}

// 生成随机物品数据
void generateRandomItems() {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        items[i].weight = 1 + rand() % 100;              // 重量1-100
        items[i].value = 100 + (rand() % 900) + (rand() % 100) / 100.0; // 价值100-999.99
    }
}

// 测试大规模数据集
void runLargeScaleTests() {
    int test_n[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 
                   10000, 20000, 40000, 80000, 160000, 320000};
    int test_capacity[] = {10000, 100000, 1000000};
    int num_n = sizeof(test_n)/sizeof(test_n[0]);
    int num_cap = sizeof(test_capacity)/sizeof(test_capacity[0]);

    // 打开CSV文件准备写入
    FILE *csv_file = fopen("动态规划_大规模数据测试结果.csv", "w");
    if (csv_file == NULL) {
        printf("无法创建CSV文件!\n");
        return;
    }
    
    // 写入CSV文件头
    fprintf(csv_file, "物品数量,背包容量,最大价值,计算时间(s)\n");

    printf("\n大规模测试开始...\n");
    printf("| %-10s | %-10s | %-15s | %-15s |\n", "物品数量", "背包容量", "最大价值", "计算时间(s)");
    printf("|------------|------------|-----------------|-----------------|\n");

    for (int i = 0; i < num_n; i++) {
        for (int j = 0; j < num_cap; j++) {
            n = test_n[i];
            capacity = test_capacity[j];
            
            generateRandomItems();
            
            clock_t start = clock();
            dynamicProgrammingLarge();
            clock_t end = clock();
            
            double time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
            
            // 输出到控制台
            printf("| %-10d | %-10d | %-15.2f | %-15.3f |\n", 
                  n, capacity, max_value, time_used);
            
            // 写入CSV文件
            fprintf(csv_file, "%d,%d,%.2f,%.3f\n", 
                   n, capacity, max_value, time_used);
        }
    }
    
    fclose(csv_file);
    printf("\n测试结果已保存到 动态规划_大规模数据测试结果.csv 文件中\n");
}

int main() {
    printf("动态规划的大规模测试！\n");
    runLargeScaleTests();
    return 0;
}
