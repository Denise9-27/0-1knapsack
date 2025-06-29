#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#define MAX_N 320000 // 最大物品数量
#define MAX_W 1000000 // 最大背包容量
#define MAX_DP_CAPACITY 100000 // 动态规划的最大容量限制
#define MAX_BACKTRACK_N 50  // 回溯法最大物品数量限制
#define MAX_BRUTE_N 25 // 蛮力法最大物品数量限制
typedef struct {
    int weight;
    double value;
} Item;

Item items[MAX_N];
int n;
int capacity;
double max_value;
FILE *fp_special = NULL;

// 函数声明
void clearInputBuffer();
bool isNumber(const char* str);
bool parseItemInput(const char* input, int* weight, int* value);
void showItems();
void saveItemsToFile(const char* filename);
void dynamicProgramming();
void bruteForce();
void greedyAlgorithm();
void backtrackHelper(int level, double current_weight, double current_value,
                    double* max_val, int* selected, int* best_selected);
void backtracking();
void inputCapacity();
void inputItemCount();
void inputItems();
void generateRandomItems();
void heapSort(double arr[], int idx[], int n);
void runLargeScaleTests();

// 清除输入缓冲区
void clearInputBuffer() {
    while (getchar() != '\n');
}

// 检查字符串是否只包含数字
bool isNumber(const char* str) {
    if (str == NULL || *str == '\0') return false;
    while (*str) {
        if (!isdigit(*str)) return false;
        str++;
    }
    return true;
}

// 解析逗号分隔的输入
bool parseItemInput(const char* input, int* weight, int* value) {
    char* token;
    char* endptr;
    char inputCopy[50];
    strcpy(inputCopy, input);

    token = strtok(inputCopy, ",");
    if (token == NULL) {
        strcpy(inputCopy, input);
        token = strtok(inputCopy, "，");
    }
    if (token == NULL) return false;

    *weight = strtol(token, &endptr, 10);
    if (*endptr != '\0' || *weight <= 0) return false;

    token = strtok(NULL, ",");
    if (token == NULL) {
        strcpy(inputCopy, input);
        strtok(inputCopy, "，");
        token = strtok(NULL, "，");
    }
    if (token == NULL) return false;

    *value = strtol(token, &endptr, 10);
    if (*endptr != '\0' || *value <= 0) return false;

    return true;
}

// 显示当前物品信息
void showItems() {
    printf("\n当前物品信息:\n");
    printf("物品数量: %d\n", n);
    printf("背包容量: %d\n", capacity);
    printf("物品编号\t重量\t价值\t价值密度\n");

    int displayCount = n > 1000 ? 1000 : n;
    for (int i = 0; i < displayCount; i++) {
        printf("%d\t\t%d\t%.2f\t%.2f\n", i + 1, items[i].weight,
               items[i].value, items[i].value / items[i].weight);
    }
    if (n > 1000) {
        printf("... (共%d个物品，只显示前1000个)\n", n);
    }
}

// 保存物品信息到文件
void saveItemsToFile(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法创建文件 %s\n", filename);
        return;
    }

    fprintf(fp, "物品编号,重量,价值\n");
    int saveCount = n > 1000 ? 1000 : n;
    for (int i = 0; i < saveCount; i++) {
        fprintf(fp, "%d,%d,%.2f\n", i+1, items[i].weight, items[i].value);
    }
    fclose(fp);
    printf("物品信息已保存到 %s\n", filename);
}

// 动态规划法求解
void dynamicProgramming() {
    if (capacity > MAX_DP_CAPACITY) {
        printf("\n动态规划法求解结果:\n");
        printf("警告: 背包容量过大(%d > %d)，动态规划法可能消耗过多内存!\n",
               capacity, MAX_DP_CAPACITY);
        printf("建议使用其他算法或减小背包容量。\n");
        return;
    }

    clock_t startTime = clock();
    
    double** dp = (double**)malloc((n + 1) * sizeof(double*));
    for (int i = 0; i <= n; i++) {
        dp[i] = (double*)malloc((capacity + 1) * sizeof(double));
        memset(dp[i], 0, (capacity + 1) * sizeof(double));
    }

    for (int i = 1; i <= n; i++) {
        for (int w = 1; w <= capacity; w++) {
            dp[i][w] = dp[i-1][w];
            
            if (items[i-1].weight <= w) {
                double valueIfSelected = dp[i-1][w - items[i-1].weight] + items[i-1].value;
                if (valueIfSelected > dp[i][w]) {
                    dp[i][w] = valueIfSelected;
                }
            }
        }
    }

    max_value = dp[n][capacity];

    int* selected = (int*)calloc(n, sizeof(int));
    double total_weight = 0;
    int w = capacity;
    for (int i = n; i > 0; i--) {
        if (dp[i][w] != dp[i-1][w]) {
            selected[i-1] = 1;
            w -= items[i-1].weight;
            total_weight += items[i-1].weight;
        }
    }

    clock_t endTime = clock();
    double time_used = ((double)(endTime - startTime)) / CLOCKS_PER_SEC * 1000;

    printf("\n动态规划法求解结果:\n");
    printf("最大价值: %.2f\n", max_value);
    printf("计算时间: %.2f 毫秒\n", time_used);
    printf("选择的物品编号: ");
    for (int i = 0; i < n; i++) {
        if (selected[i]) {
            printf("%d ", i + 1);
        }
    }
    
    printf("\n\n选择的物品详细信息:\n");
    printf("物品编号\t重量\t价值\n");
    for (int i = 0; i < n; i++) {
        if (selected[i]) {
            printf("%d\t\t%d\t%.2f\n", i + 1, items[i].weight, items[i].value);
        }
    }
    printf("\n总重量: %.2f (剩余容量: %.2f)\n", total_weight, capacity - total_weight);

    // 保存所有物品信息
    if (n == 20 && capacity == 10000) {
        fp_special = fopen("special_results.csv", "w");
        if (fp_special) {
            fprintf(fp_special, "算法类型,动态规划法\n");
            fprintf(fp_special, "物品数量,%d\n", n);
            fprintf(fp_special, "背包容量,%d\n\n", capacity);
            fprintf(fp_special, "最大价值,%.2f\n", max_value);
            fprintf(fp_special, "计算时间(毫秒),%.2f\n", time_used);
            
            fprintf(fp_special, "\n物品编号,重量,价值,是否选中\n");
            for (int i = 0; i < n; i++) {
                fprintf(fp_special, "%d,%d,%.2f,%d\n", 
                       i + 1, items[i].weight, items[i].value, selected[i]);
            }
            fclose(fp_special);
            fp_special = NULL;
            printf("\n动态规划法详细结果已保存到 special_results.csv\n");
        }
    }

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);
    free(selected);
}

// 蛮力法求解
void bruteForce() {
    clock_t start = clock();
    int i, j;
    double max_val = 0;
    double total_weight;
    double total_value;
    int best_combination = 0;
    
    // 遍历所有可能的组合 (2^n种可能)
    for (i = 0; i < (1 << n); i++) {
        total_weight = 0;
        total_value = 0;
        for (j = 0; j < n; j++) {
            if (i & (1 << j)) {
                total_weight += items[j].weight;
                total_value += items[j].value;
            }
        }

        // 如果当前组合满足重量限制且价值更大，则更新最大值
        if (total_weight <= capacity && total_value > max_val) {
            max_val = total_value;
            best_combination = i;
        }
    }

    max_value = max_val;
    clock_t end = clock();
    double time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

    // 打印结果
    printf("\n蛮力法求解结果:\n");
    printf("最大价值: %.2f\n", max_value);
    printf("计算时间: %.2f 毫秒\n", time_used);

    // 显示选择的物品详细信息
    printf("\n选择的物品详细信息:\n");
    printf("物品编号\t重量\t价值\n");
    double total_w = 0;
    int* selected = (int*)calloc(n, sizeof(int));
    for (i = 0; i < n; i++) {
        if (best_combination & (1 << i)) {
            printf("%d\t\t%d\t%.2f\n", i+1, items[i].weight, items[i].value);
            total_w += items[i].weight;
            selected[i] = 1;
        }
    }
    printf("\n总重量: %.2f (剩余容量: %.2f)\n", total_w, capacity - total_w);

    // 保存所有物品信息
    if (n == 20 && capacity == 10000) {
        fp_special = fopen("special_results.csv", "a");  // 追加模式
        if (fp_special) {
            fprintf(fp_special, "\n\n算法类型,蛮力法\n");
            fprintf(fp_special, "物品数量,%d\n", n);
            fprintf(fp_special, "背包容量,%d\n\n", capacity);
            fprintf(fp_special, "最大价值,%.2f\n", max_value);
            fprintf(fp_special, "计算时间(毫秒),%.2f\n", time_used);
            
            fprintf(fp_special, "\n物品编号,重量,价值,是否选中\n");
            for (int i = 0; i < n; i++) {
                fprintf(fp_special, "%d,%d,%.2f,%d\n", 
                       i + 1, items[i].weight, items[i].value, selected[i]);
            }
            fclose(fp_special);
            fp_special = NULL;
            printf("\n蛮力法详细结果已追加到 special_results.csv\n");
        }
    }
    
    free(selected);
}

// 堆排序辅助函数
void heapify(double arr[], int idx[], int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;

    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != i) {
        double temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;

        int temp_idx = idx[i];
        idx[i] = idx[largest];
        idx[largest] = temp_idx;

        heapify(arr, idx, n, largest);
    }
}

// 堆排序
void heapSort(double arr[], int idx[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, idx, n, i);

    for (int i = n - 1; i > 0; i--) {
        double temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        int temp_idx = idx[0];
        idx[0] = idx[i];
        idx[i] = temp_idx;

        heapify(arr, idx, i, 0);
    }
}

// 贪心算法
void greedyAlgorithm() {
    if (n <= 0 || capacity <= 0) {
        printf("无效的物品数量或背包容量!\n");
        return;
    }

    clock_t start = clock();

    // 创建索引数组并计算价值密度
    int *index = (int *)malloc(n * sizeof(int));
    double *density = (double *)malloc(n * sizeof(double));
    if (!index || !density) {
        printf("内存分配失败!\n");
        free(index);
        free(density);
        return;
    }

    for (int i = 0; i < n; i++) {
        index[i] = i;
        density[i] = items[i].value / items[i].weight;
    }

    // 使用堆排序按价值密度降序排序
    heapSort(density, index, n);

    double current_weight = 0;
    max_value = 0;
    int *selected = (int *)calloc(n, sizeof(int));
    if (!selected) {
        printf("内存分配失败!\n");
        free(index);
        free(density);
        return;
    }

    // 选择物品
    for (int i = n-1; i >= 0; i--) {
        if (current_weight + items[index[i]].weight <= capacity) {
            selected[index[i]] = 1;
            current_weight += items[index[i]].weight;
            max_value += items[index[i]].value;
        }
    }

    clock_t end = clock();
    double time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

    // 打印结果
    printf("\n贪心算法求解结果:\n");
    printf("最大价值: %.2f\n", max_value);
    printf("计算时间: %.2f 毫秒\n", time_used);

    printf("\n选择的物品详细信息:\n");
    printf("物品编号\t重量\t价值\n");
    for (int i = 0; i < n; i++) {
        if (selected[i]) {
            printf("%d\t\t%d\t%.2f\n", i + 1, items[i].weight, items[i].value);
        }
    }
    printf("\n总重量: %.2f (剩余容量: %.2f)\n", current_weight, capacity - current_weight);

    // 保存所有物品信息
    if (n == 20 && capacity == 10000) {
        fp_special = fopen("special_results.csv", "a");  // 追加模式
        if (fp_special) {
            fprintf(fp_special, "\n\n算法类型,贪心算法\n");
            fprintf(fp_special, "物品数量,%d\n", n);
            fprintf(fp_special, "背包容量,%d\n\n", capacity);
            fprintf(fp_special, "最大价值,%.2f\n", max_value);
            fprintf(fp_special, "计算时间(毫秒),%.2f\n", time_used);
            
            fprintf(fp_special, "\n物品编号,重量,价值,是否选中\n");
            for (int i = 0; i < n; i++) {
                fprintf(fp_special, "%d,%d,%.2f,%d\n", 
                       i + 1, items[i].weight, items[i].value, selected[i]);
            }
            fclose(fp_special);
            fp_special = NULL;
            printf("\n贪心算法详细结果已追加到 special_results.csv\n");
        }
    }

    // 释放内存
    free(index);
    free(density);
    free(selected);
}

// 回溯法辅助函数
void backtrackHelper(int level, double current_weight, double current_value,
                    double* max_val, int* selected, int* best_selected) {
    if (level == n) {
        if (current_value > *max_val) {
            *max_val = current_value;
            for (int i = 0; i < n; i++) {
                best_selected[i] = selected[i];
            }
        }
        return;
    }

    // 剪枝：如果当前价值加上剩余物品的最大可能价值仍小于已知最大值，则剪枝
    double remaining_bound = current_value;
    double remaining_capacity = capacity - current_weight;
    for (int i = level; i < n && remaining_capacity > 0; i++) {
        if (items[i].weight <= remaining_capacity) {
            remaining_bound += items[i].value;
            remaining_capacity -= items[i].weight;
        } else {
            remaining_bound += items[i].value * (remaining_capacity / items[i].weight);
            break;
        }
    }
    if (remaining_bound <= *max_val) return;

    // 优先探索更有希望的路径（按价值密度排序）
    if (items[level].weight <= capacity - current_weight) {
        selected[level] = 1;
        backtrackHelper(level + 1, 
                       current_weight + items[level].weight,
                       current_value + items[level].value,
                       max_val, selected, best_selected);
    }

    selected[level] = 0;
    backtrackHelper(level + 1, current_weight, current_value, max_val, selected, best_selected);
}

// 回溯法
void backtracking() {
    // 按价值密度降序排序
    for (int i = 0; i < n-1; i++) {
        for (int j = i+1; j < n; j++) {
            double density_i = items[i].value / items[i].weight;
            double density_j = items[j].value / items[j].weight;
            if (density_i < density_j) {
                Item temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }

    clock_t start = clock();

    int *selected = (int *)calloc(n, sizeof(int));
    int *best_selected = (int *)calloc(n, sizeof(int));
    if (!selected || !best_selected) {
        printf("内存分配失败!\n");
        free(selected);
        free(best_selected);
        return;
    }

    double current_max = 0;
    backtrackHelper(0, 0, 0, &current_max, selected, best_selected);
    max_value = current_max;

    clock_t end = clock();
    double time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

    // 打印结果
    printf("\n回溯法(带剪枝)求解结果:\n");
    printf("最大价值: %.2f\n", max_value);
    printf("计算时间: %.2f 毫秒\n", time_used);

    printf("\n选择的物品详细信息:\n");
    printf("物品编号\t重量\t价值\n");
    double total_weight = 0;
    for (int i = 0; i < n; i++) {
        if (best_selected[i]) {
            printf("%d\t\t%d\t%.2f\n", i + 1, items[i].weight, items[i].value);
            total_weight += items[i].weight;
        }
    }
    printf("\n总重量: %.2f (剩余容量: %.2f)\n", total_weight, capacity - total_weight);

    // 保存所有物品信息
    if (n == 20 && capacity == 10000) {
        fp_special = fopen("special_results.csv", "a");  // 追加模式
        if (fp_special) {
            fprintf(fp_special, "\n\n算法类型,回溯法\n");
            fprintf(fp_special, "物品数量,%d\n", n);
            fprintf(fp_special, "背包容量,%d\n\n", capacity);
            fprintf(fp_special, "最大价值,%.2f\n", max_value);
            fprintf(fp_special, "计算时间(毫秒),%.2f\n", time_used);
            
            fprintf(fp_special, "\n物品编号,重量,价值,是否选中\n");
            for (int i = 0; i < n; i++) {
                fprintf(fp_special, "%d,%d,%.2f,%d\n", 
                       i + 1, items[i].weight, items[i].value, best_selected[i]);
            }
            fclose(fp_special);
            fp_special = NULL;
            printf("\n回溯法详细结果已追加到 special_results.csv\n");
        }
    }

    // 释放内存
    free(selected);
    free(best_selected);
}


// 输入背包容量
void inputCapacity() {
    char input[20];
    while (1) {
        printf("请输入背包容量(1-%d): ", MAX_W);
        if (fgets(input, sizeof(input), stdin)){
            input[strcspn(input, "\n")] = '\0';
            if (!isNumber(input)) {
                printf("错误: 请输入有效的正整数!\n");
                continue;
            }
            capacity = atoi(input);
            if (capacity <= 0 || capacity > MAX_W) {
                printf("错误: 背包容量必须在1到%d之间!\n", MAX_W);
                continue;
            }
            break;
        }
    }
}

// 输入物品数量
void inputItemCount() {
    char input[20];
    while (1) {
        printf("请输入物品数量(1-%d): ", MAX_N);
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0';
            if (!isNumber(input)) {
                printf("错误: 请输入有效的正整数!\n");
                continue;
            }
            n = atoi(input);
            if (n <= 0 || n > MAX_N) {
                printf("错误: 物品数量必须在1到%d之间!\n", MAX_N);
                continue;
            }
            break;
        }
    }
}

// 输入物品信息
void inputItems() {
    char input[50];
    int weight, value;
    printf("请依次输入每个物品的重量和价值(格式: 重量,价值):\n");
    for (int i = 0; i < n; i++) {
        while (1) {
            printf("物品%d: ", i + 1);
            if (fgets(input, sizeof(input), stdin) != NULL){
                input[strcspn(input, "\n")] = '\0';
                if (!parseItemInput(input, &weight, &value)) {
                    printf("错误: 请输入有效的重量和价值(格式: 重量,价值)!\n");
                    continue;
                }
                items[i].weight = weight;
                items[i].value = (double)value;
                break;
            }
        }
    }
}

void generateRandomItems() {
    srand(time(NULL));
    
    // 根据背包容量确定重量范围
    int min_weight, max_weight;
    if (capacity > 100) {
        min_weight = 601;  // 重量>600
        max_weight = 1000; // 假设最大重量1000
    } else {
        min_weight = 1;    // 默认1-100
        max_weight = 100;
    }

    for (int i = 0; i < n; i++) {
        items[i].weight = min_weight + rand() % (max_weight - min_weight + 1);
        items[i].value = 100 + (rand() % 900) + (rand() % 100) / 100.0;
    }
    
    // 验证总重量是否合理（可选）
    int total_weight = 0;
    for (int i = 0; i < n; i++) {
        total_weight += items[i].weight;
    }
}

// 测试大规模数据集
void runLargeScaleTests() {
    int test_n[] = {10,12,15,18,20, 25,50, 100, 200, 500, 1000, 2000, 5000, 10000};
    int test_capacity[] = {100, 1000, 5000, 10000, 100000};
    int num_n = sizeof(test_n)/sizeof(test_n[0]);
    int num_cap = sizeof(test_capacity)/sizeof(test_capacity[0]);

    printf("\n大规模测试开始...\n");
    printf("| %-10s | %-10s | %-15s | %-15s | %-15s | %-15s | %-15s | %-15s | %-15s | %-15s |\n",
           "物品数量", "背包容量", "蛮力法价值", "蛮力法时间",
           "动态规划价值", "动态规划时间", "贪心算法价值", "贪心算法时间", 
           "回溯法价值", "回溯法时间");
    printf("|-----------|-----------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|\n");

    FILE *fp = fopen("performance_results.csv", "w");
    if (fp) {
        fprintf(fp, "物品数量,背包容量,蛮力法价值,蛮力法时间(ms),动态规划价值,动态规划时间(ms),贪心算法价值,贪心算法时间(ms),回溯法价值,回溯法时间(ms)\n");
    }

    for (int i = 0; i < num_n; i++) {
        for (int j = 0; j < num_cap; j++) {
            n = test_n[i];
            capacity = test_capacity[j];
            generateRandomItems();

            double brute_value = 0, dp_value = 0, greedy_value = 0, backtrack_value = 0;
            double brute_time = 0, dp_time = 0, greedy_time = 0, backtrack_time = 0;

            // 保存原始max_value
            double original_max_value = max_value;
			
			//蛮力法（仅当n<=20时运行） 
			if(n <= MAX_BRUTE_N) {
				clock_t brute_start = clock();
	            bruteForce();
	            brute_value = max_value;
	            brute_time = ((double)(clock() - brute_start)) / CLOCKS_PER_SEC * 1000;
	            max_value = original_max_value;
			}else{
				brute_value = 0;
				brute_time = 0;
			}
            // 动态规划
            if (capacity <= MAX_DP_CAPACITY) {
                clock_t dp_start = clock();
                dynamicProgramming();
                dp_value = max_value;
                dp_time = ((double)(clock() - dp_start)) / CLOCKS_PER_SEC * 1000;
                max_value = original_max_value;
            }
			
            // 贪心算法
            clock_t greedy_start = clock();
            greedyAlgorithm();
            greedy_value = max_value;
            greedy_time = ((double)(clock() - greedy_start)) / CLOCKS_PER_SEC * 1000;
            max_value = original_max_value;
			
			// 回溯法（仅当n<=50时运行）
			if (n <= MAX_BACKTRACK_N) {
			    clock_t backtrack_start = clock();
			    backtracking();
			    clock_t backtrack_end = clock();
			    backtrack_value = max_value;
			    backtrack_time = ((double)(backtrack_end - backtrack_start)) / CLOCKS_PER_SEC * 1000;
			    max_value = original_max_value;
			} else {
			    backtrack_value = 0;
			    backtrack_time = 0;
			}

            // 打印结果
            printf("| %-10d | %-10d | %-15.2f | %-15.3f | %-15.2f | %-15.3f | %-15.2f | %-15.3f | %-15.2f | %-15.3f |\n",
                   n, capacity, brute_value, brute_time, dp_value, dp_time, 
                   greedy_value, greedy_time, backtrack_value, backtrack_time);

            if (fp) {
                fprintf(fp, "%d,%d,%.2f,%.3f,%.2f,%.3f,%.2f,%.3f,%.2f,%.3f\n",
                        n, capacity, brute_value, brute_time, dp_value, dp_time, 
                        greedy_value, greedy_time, backtrack_value, backtrack_time);
            }

            printf("已完成: 物品数量=%d, 背包容量=%d\n", n, capacity);
        }
    }

    if (fp) {
        fclose(fp);
        printf("\n测试结果已保存到 performance_results.csv\n");
    }
}

int main() {
    printf("=== 0-1背包问题求解程序 ===\n");
    printf("支持物品数量最多%d个，背包容量最大%d\n", MAX_N, MAX_W);

    int mainChoice;
    while (1) {
        printf("\n主菜单:\n");
        printf("1. 手动输入数据求解\n");
        printf("2. 运行大规模测试\n");
        printf("3. 退出程序\n");
        printf("请选择操作: ");

        if (scanf("%d", &mainChoice) != 1) {
            clearInputBuffer();
            printf("无效的输入! 请输入数字1-3。\n");
            continue;
        }
        clearInputBuffer();

        switch (mainChoice) {
            case 1: {
                inputCapacity();
                inputItemCount();
                inputItems();
                saveItemsToFile("items_info.csv");

                int algoChoice;
                do {
                    printf("\n算法选择菜单:\n");
                    printf("1. 蛮力法求解\n");
                    printf("2. 动态规划法求解\n");
                    printf("3. 贪心算法求解\n");
                    printf("4. 回溯法(带剪枝)求解\n");
                    printf("5. 显示当前物品信息\n");
                    printf("6. 重新输入数据\n");
                    printf("7. 返回主菜单\n");
                    printf("请选择操作: ");

                    if (scanf("%d", &algoChoice) != 1) {
                        clearInputBuffer();
                        printf("无效的输入! 请输入数字1-7。\n");
                        continue;
                    }
                    clearInputBuffer();

                    switch (algoChoice) {
                        case 1: bruteForce(); break;
                        case 2: dynamicProgramming(); break;
                        case 3: greedyAlgorithm(); break;
                        case 4: backtracking(); break;
                        case 5: showItems(); break;
                        case 6: 
                            inputCapacity();
                            inputItemCount();
                            inputItems();
                            saveItemsToFile("items_info.csv");
                            break;
                        case 7: break;
                        default: printf("无效的选择! 请输入数字1-7。\n");
                    }
                } while (algoChoice != 7);
                break;
            }
            case 2:
                runLargeScaleTests();
                break;
            case 3:
                printf("程序退出。\n");
                return 0;
            default:
                printf("无效的选择! 请输入数字1-3。\n");
        }
    }
    return 0;
}
