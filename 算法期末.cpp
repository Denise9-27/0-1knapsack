#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#define MAX_N 320000 // �����Ʒ����
#define MAX_W 1000000 // ��󱳰�����
#define MAX_DP_CAPACITY 100000 // ��̬�滮�������������
#define MAX_BACKTRACK_N 50  // ���ݷ������Ʒ��������
#define MAX_BRUTE_N 25 // �����������Ʒ��������
typedef struct {
    int weight;
    double value;
} Item;

Item items[MAX_N];
int n;
int capacity;
double max_value;
FILE *fp_special = NULL;

// ��������
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

// ������뻺����
void clearInputBuffer() {
    while (getchar() != '\n');
}

// ����ַ����Ƿ�ֻ��������
bool isNumber(const char* str) {
    if (str == NULL || *str == '\0') return false;
    while (*str) {
        if (!isdigit(*str)) return false;
        str++;
    }
    return true;
}

// �������ŷָ�������
bool parseItemInput(const char* input, int* weight, int* value) {
    char* token;
    char* endptr;
    char inputCopy[50];
    strcpy(inputCopy, input);

    token = strtok(inputCopy, ",");
    if (token == NULL) {
        strcpy(inputCopy, input);
        token = strtok(inputCopy, "��");
    }
    if (token == NULL) return false;

    *weight = strtol(token, &endptr, 10);
    if (*endptr != '\0' || *weight <= 0) return false;

    token = strtok(NULL, ",");
    if (token == NULL) {
        strcpy(inputCopy, input);
        strtok(inputCopy, "��");
        token = strtok(NULL, "��");
    }
    if (token == NULL) return false;

    *value = strtol(token, &endptr, 10);
    if (*endptr != '\0' || *value <= 0) return false;

    return true;
}

// ��ʾ��ǰ��Ʒ��Ϣ
void showItems() {
    printf("\n��ǰ��Ʒ��Ϣ:\n");
    printf("��Ʒ����: %d\n", n);
    printf("��������: %d\n", capacity);
    printf("��Ʒ���\t����\t��ֵ\t��ֵ�ܶ�\n");

    int displayCount = n > 1000 ? 1000 : n;
    for (int i = 0; i < displayCount; i++) {
        printf("%d\t\t%d\t%.2f\t%.2f\n", i + 1, items[i].weight,
               items[i].value, items[i].value / items[i].weight);
    }
    if (n > 1000) {
        printf("... (��%d����Ʒ��ֻ��ʾǰ1000��)\n", n);
    }
}

// ������Ʒ��Ϣ���ļ�
void saveItemsToFile(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("�޷������ļ� %s\n", filename);
        return;
    }

    fprintf(fp, "��Ʒ���,����,��ֵ\n");
    int saveCount = n > 1000 ? 1000 : n;
    for (int i = 0; i < saveCount; i++) {
        fprintf(fp, "%d,%d,%.2f\n", i+1, items[i].weight, items[i].value);
    }
    fclose(fp);
    printf("��Ʒ��Ϣ�ѱ��浽 %s\n", filename);
}

// ��̬�滮�����
void dynamicProgramming() {
    if (capacity > MAX_DP_CAPACITY) {
        printf("\n��̬�滮�������:\n");
        printf("����: ������������(%d > %d)����̬�滮���������Ĺ����ڴ�!\n",
               capacity, MAX_DP_CAPACITY);
        printf("����ʹ�������㷨���С����������\n");
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

    printf("\n��̬�滮�������:\n");
    printf("����ֵ: %.2f\n", max_value);
    printf("����ʱ��: %.2f ����\n", time_used);
    printf("ѡ�����Ʒ���: ");
    for (int i = 0; i < n; i++) {
        if (selected[i]) {
            printf("%d ", i + 1);
        }
    }
    
    printf("\n\nѡ�����Ʒ��ϸ��Ϣ:\n");
    printf("��Ʒ���\t����\t��ֵ\n");
    for (int i = 0; i < n; i++) {
        if (selected[i]) {
            printf("%d\t\t%d\t%.2f\n", i + 1, items[i].weight, items[i].value);
        }
    }
    printf("\n������: %.2f (ʣ������: %.2f)\n", total_weight, capacity - total_weight);

    // ����������Ʒ��Ϣ
    if (n == 20 && capacity == 10000) {
        fp_special = fopen("special_results.csv", "w");
        if (fp_special) {
            fprintf(fp_special, "�㷨����,��̬�滮��\n");
            fprintf(fp_special, "��Ʒ����,%d\n", n);
            fprintf(fp_special, "��������,%d\n\n", capacity);
            fprintf(fp_special, "����ֵ,%.2f\n", max_value);
            fprintf(fp_special, "����ʱ��(����),%.2f\n", time_used);
            
            fprintf(fp_special, "\n��Ʒ���,����,��ֵ,�Ƿ�ѡ��\n");
            for (int i = 0; i < n; i++) {
                fprintf(fp_special, "%d,%d,%.2f,%d\n", 
                       i + 1, items[i].weight, items[i].value, selected[i]);
            }
            fclose(fp_special);
            fp_special = NULL;
            printf("\n��̬�滮����ϸ����ѱ��浽 special_results.csv\n");
        }
    }

    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);
    free(selected);
}

// ���������
void bruteForce() {
    clock_t start = clock();
    int i, j;
    double max_val = 0;
    double total_weight;
    double total_value;
    int best_combination = 0;
    
    // �������п��ܵ���� (2^n�ֿ���)
    for (i = 0; i < (1 << n); i++) {
        total_weight = 0;
        total_value = 0;
        for (j = 0; j < n; j++) {
            if (i & (1 << j)) {
                total_weight += items[j].weight;
                total_value += items[j].value;
            }
        }

        // �����ǰ����������������Ҽ�ֵ������������ֵ
        if (total_weight <= capacity && total_value > max_val) {
            max_val = total_value;
            best_combination = i;
        }
    }

    max_value = max_val;
    clock_t end = clock();
    double time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

    // ��ӡ���
    printf("\n�����������:\n");
    printf("����ֵ: %.2f\n", max_value);
    printf("����ʱ��: %.2f ����\n", time_used);

    // ��ʾѡ�����Ʒ��ϸ��Ϣ
    printf("\nѡ�����Ʒ��ϸ��Ϣ:\n");
    printf("��Ʒ���\t����\t��ֵ\n");
    double total_w = 0;
    int* selected = (int*)calloc(n, sizeof(int));
    for (i = 0; i < n; i++) {
        if (best_combination & (1 << i)) {
            printf("%d\t\t%d\t%.2f\n", i+1, items[i].weight, items[i].value);
            total_w += items[i].weight;
            selected[i] = 1;
        }
    }
    printf("\n������: %.2f (ʣ������: %.2f)\n", total_w, capacity - total_w);

    // ����������Ʒ��Ϣ
    if (n == 20 && capacity == 10000) {
        fp_special = fopen("special_results.csv", "a");  // ׷��ģʽ
        if (fp_special) {
            fprintf(fp_special, "\n\n�㷨����,������\n");
            fprintf(fp_special, "��Ʒ����,%d\n", n);
            fprintf(fp_special, "��������,%d\n\n", capacity);
            fprintf(fp_special, "����ֵ,%.2f\n", max_value);
            fprintf(fp_special, "����ʱ��(����),%.2f\n", time_used);
            
            fprintf(fp_special, "\n��Ʒ���,����,��ֵ,�Ƿ�ѡ��\n");
            for (int i = 0; i < n; i++) {
                fprintf(fp_special, "%d,%d,%.2f,%d\n", 
                       i + 1, items[i].weight, items[i].value, selected[i]);
            }
            fclose(fp_special);
            fp_special = NULL;
            printf("\n��������ϸ�����׷�ӵ� special_results.csv\n");
        }
    }
    
    free(selected);
}

// ������������
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

// ������
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

// ̰���㷨
void greedyAlgorithm() {
    if (n <= 0 || capacity <= 0) {
        printf("��Ч����Ʒ�����򱳰�����!\n");
        return;
    }

    clock_t start = clock();

    // �����������鲢�����ֵ�ܶ�
    int *index = (int *)malloc(n * sizeof(int));
    double *density = (double *)malloc(n * sizeof(double));
    if (!index || !density) {
        printf("�ڴ����ʧ��!\n");
        free(index);
        free(density);
        return;
    }

    for (int i = 0; i < n; i++) {
        index[i] = i;
        density[i] = items[i].value / items[i].weight;
    }

    // ʹ�ö����򰴼�ֵ�ܶȽ�������
    heapSort(density, index, n);

    double current_weight = 0;
    max_value = 0;
    int *selected = (int *)calloc(n, sizeof(int));
    if (!selected) {
        printf("�ڴ����ʧ��!\n");
        free(index);
        free(density);
        return;
    }

    // ѡ����Ʒ
    for (int i = n-1; i >= 0; i--) {
        if (current_weight + items[index[i]].weight <= capacity) {
            selected[index[i]] = 1;
            current_weight += items[index[i]].weight;
            max_value += items[index[i]].value;
        }
    }

    clock_t end = clock();
    double time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

    // ��ӡ���
    printf("\n̰���㷨�����:\n");
    printf("����ֵ: %.2f\n", max_value);
    printf("����ʱ��: %.2f ����\n", time_used);

    printf("\nѡ�����Ʒ��ϸ��Ϣ:\n");
    printf("��Ʒ���\t����\t��ֵ\n");
    for (int i = 0; i < n; i++) {
        if (selected[i]) {
            printf("%d\t\t%d\t%.2f\n", i + 1, items[i].weight, items[i].value);
        }
    }
    printf("\n������: %.2f (ʣ������: %.2f)\n", current_weight, capacity - current_weight);

    // ����������Ʒ��Ϣ
    if (n == 20 && capacity == 10000) {
        fp_special = fopen("special_results.csv", "a");  // ׷��ģʽ
        if (fp_special) {
            fprintf(fp_special, "\n\n�㷨����,̰���㷨\n");
            fprintf(fp_special, "��Ʒ����,%d\n", n);
            fprintf(fp_special, "��������,%d\n\n", capacity);
            fprintf(fp_special, "����ֵ,%.2f\n", max_value);
            fprintf(fp_special, "����ʱ��(����),%.2f\n", time_used);
            
            fprintf(fp_special, "\n��Ʒ���,����,��ֵ,�Ƿ�ѡ��\n");
            for (int i = 0; i < n; i++) {
                fprintf(fp_special, "%d,%d,%.2f,%d\n", 
                       i + 1, items[i].weight, items[i].value, selected[i]);
            }
            fclose(fp_special);
            fp_special = NULL;
            printf("\n̰���㷨��ϸ�����׷�ӵ� special_results.csv\n");
        }
    }

    // �ͷ��ڴ�
    free(index);
    free(density);
    free(selected);
}

// ���ݷ���������
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

    // ��֦�������ǰ��ֵ����ʣ����Ʒ�������ܼ�ֵ��С����֪���ֵ�����֦
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

    // ����̽������ϣ����·��������ֵ�ܶ�����
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

// ���ݷ�
void backtracking() {
    // ����ֵ�ܶȽ�������
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
        printf("�ڴ����ʧ��!\n");
        free(selected);
        free(best_selected);
        return;
    }

    double current_max = 0;
    backtrackHelper(0, 0, 0, &current_max, selected, best_selected);
    max_value = current_max;

    clock_t end = clock();
    double time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

    // ��ӡ���
    printf("\n���ݷ�(����֦)�����:\n");
    printf("����ֵ: %.2f\n", max_value);
    printf("����ʱ��: %.2f ����\n", time_used);

    printf("\nѡ�����Ʒ��ϸ��Ϣ:\n");
    printf("��Ʒ���\t����\t��ֵ\n");
    double total_weight = 0;
    for (int i = 0; i < n; i++) {
        if (best_selected[i]) {
            printf("%d\t\t%d\t%.2f\n", i + 1, items[i].weight, items[i].value);
            total_weight += items[i].weight;
        }
    }
    printf("\n������: %.2f (ʣ������: %.2f)\n", total_weight, capacity - total_weight);

    // ����������Ʒ��Ϣ
    if (n == 20 && capacity == 10000) {
        fp_special = fopen("special_results.csv", "a");  // ׷��ģʽ
        if (fp_special) {
            fprintf(fp_special, "\n\n�㷨����,���ݷ�\n");
            fprintf(fp_special, "��Ʒ����,%d\n", n);
            fprintf(fp_special, "��������,%d\n\n", capacity);
            fprintf(fp_special, "����ֵ,%.2f\n", max_value);
            fprintf(fp_special, "����ʱ��(����),%.2f\n", time_used);
            
            fprintf(fp_special, "\n��Ʒ���,����,��ֵ,�Ƿ�ѡ��\n");
            for (int i = 0; i < n; i++) {
                fprintf(fp_special, "%d,%d,%.2f,%d\n", 
                       i + 1, items[i].weight, items[i].value, best_selected[i]);
            }
            fclose(fp_special);
            fp_special = NULL;
            printf("\n���ݷ���ϸ�����׷�ӵ� special_results.csv\n");
        }
    }

    // �ͷ��ڴ�
    free(selected);
    free(best_selected);
}


// ���뱳������
void inputCapacity() {
    char input[20];
    while (1) {
        printf("�����뱳������(1-%d): ", MAX_W);
        if (fgets(input, sizeof(input), stdin)){
            input[strcspn(input, "\n")] = '\0';
            if (!isNumber(input)) {
                printf("����: ��������Ч��������!\n");
                continue;
            }
            capacity = atoi(input);
            if (capacity <= 0 || capacity > MAX_W) {
                printf("����: ��������������1��%d֮��!\n", MAX_W);
                continue;
            }
            break;
        }
    }
}

// ������Ʒ����
void inputItemCount() {
    char input[20];
    while (1) {
        printf("��������Ʒ����(1-%d): ", MAX_N);
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0';
            if (!isNumber(input)) {
                printf("����: ��������Ч��������!\n");
                continue;
            }
            n = atoi(input);
            if (n <= 0 || n > MAX_N) {
                printf("����: ��Ʒ����������1��%d֮��!\n", MAX_N);
                continue;
            }
            break;
        }
    }
}

// ������Ʒ��Ϣ
void inputItems() {
    char input[50];
    int weight, value;
    printf("����������ÿ����Ʒ�������ͼ�ֵ(��ʽ: ����,��ֵ):\n");
    for (int i = 0; i < n; i++) {
        while (1) {
            printf("��Ʒ%d: ", i + 1);
            if (fgets(input, sizeof(input), stdin) != NULL){
                input[strcspn(input, "\n")] = '\0';
                if (!parseItemInput(input, &weight, &value)) {
                    printf("����: ��������Ч�������ͼ�ֵ(��ʽ: ����,��ֵ)!\n");
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
    
    // ���ݱ�������ȷ��������Χ
    int min_weight, max_weight;
    if (capacity > 100) {
        min_weight = 601;  // ����>600
        max_weight = 1000; // �����������1000
    } else {
        min_weight = 1;    // Ĭ��1-100
        max_weight = 100;
    }

    for (int i = 0; i < n; i++) {
        items[i].weight = min_weight + rand() % (max_weight - min_weight + 1);
        items[i].value = 100 + (rand() % 900) + (rand() % 100) / 100.0;
    }
    
    // ��֤�������Ƿ������ѡ��
    int total_weight = 0;
    for (int i = 0; i < n; i++) {
        total_weight += items[i].weight;
    }
}

// ���Դ��ģ���ݼ�
void runLargeScaleTests() {
    int test_n[] = {10,12,15,18,20, 25,50, 100, 200, 500, 1000, 2000, 5000, 10000};
    int test_capacity[] = {100, 1000, 5000, 10000, 100000};
    int num_n = sizeof(test_n)/sizeof(test_n[0]);
    int num_cap = sizeof(test_capacity)/sizeof(test_capacity[0]);

    printf("\n���ģ���Կ�ʼ...\n");
    printf("| %-10s | %-10s | %-15s | %-15s | %-15s | %-15s | %-15s | %-15s | %-15s | %-15s |\n",
           "��Ʒ����", "��������", "��������ֵ", "������ʱ��",
           "��̬�滮��ֵ", "��̬�滮ʱ��", "̰���㷨��ֵ", "̰���㷨ʱ��", 
           "���ݷ���ֵ", "���ݷ�ʱ��");
    printf("|-----------|-----------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|\n");

    FILE *fp = fopen("performance_results.csv", "w");
    if (fp) {
        fprintf(fp, "��Ʒ����,��������,��������ֵ,������ʱ��(ms),��̬�滮��ֵ,��̬�滮ʱ��(ms),̰���㷨��ֵ,̰���㷨ʱ��(ms),���ݷ���ֵ,���ݷ�ʱ��(ms)\n");
    }

    for (int i = 0; i < num_n; i++) {
        for (int j = 0; j < num_cap; j++) {
            n = test_n[i];
            capacity = test_capacity[j];
            generateRandomItems();

            double brute_value = 0, dp_value = 0, greedy_value = 0, backtrack_value = 0;
            double brute_time = 0, dp_time = 0, greedy_time = 0, backtrack_time = 0;

            // ����ԭʼmax_value
            double original_max_value = max_value;
			
			//������������n<=20ʱ���У� 
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
            // ��̬�滮
            if (capacity <= MAX_DP_CAPACITY) {
                clock_t dp_start = clock();
                dynamicProgramming();
                dp_value = max_value;
                dp_time = ((double)(clock() - dp_start)) / CLOCKS_PER_SEC * 1000;
                max_value = original_max_value;
            }
			
            // ̰���㷨
            clock_t greedy_start = clock();
            greedyAlgorithm();
            greedy_value = max_value;
            greedy_time = ((double)(clock() - greedy_start)) / CLOCKS_PER_SEC * 1000;
            max_value = original_max_value;
			
			// ���ݷ�������n<=50ʱ���У�
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

            // ��ӡ���
            printf("| %-10d | %-10d | %-15.2f | %-15.3f | %-15.2f | %-15.3f | %-15.2f | %-15.3f | %-15.2f | %-15.3f |\n",
                   n, capacity, brute_value, brute_time, dp_value, dp_time, 
                   greedy_value, greedy_time, backtrack_value, backtrack_time);

            if (fp) {
                fprintf(fp, "%d,%d,%.2f,%.3f,%.2f,%.3f,%.2f,%.3f,%.2f,%.3f\n",
                        n, capacity, brute_value, brute_time, dp_value, dp_time, 
                        greedy_value, greedy_time, backtrack_value, backtrack_time);
            }

            printf("�����: ��Ʒ����=%d, ��������=%d\n", n, capacity);
        }
    }

    if (fp) {
        fclose(fp);
        printf("\n���Խ���ѱ��浽 performance_results.csv\n");
    }
}

int main() {
    printf("=== 0-1�������������� ===\n");
    printf("֧����Ʒ�������%d���������������%d\n", MAX_N, MAX_W);

    int mainChoice;
    while (1) {
        printf("\n���˵�:\n");
        printf("1. �ֶ������������\n");
        printf("2. ���д��ģ����\n");
        printf("3. �˳�����\n");
        printf("��ѡ�����: ");

        if (scanf("%d", &mainChoice) != 1) {
            clearInputBuffer();
            printf("��Ч������! ����������1-3��\n");
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
                    printf("\n�㷨ѡ��˵�:\n");
                    printf("1. ���������\n");
                    printf("2. ��̬�滮�����\n");
                    printf("3. ̰���㷨���\n");
                    printf("4. ���ݷ�(����֦)���\n");
                    printf("5. ��ʾ��ǰ��Ʒ��Ϣ\n");
                    printf("6. ������������\n");
                    printf("7. �������˵�\n");
                    printf("��ѡ�����: ");

                    if (scanf("%d", &algoChoice) != 1) {
                        clearInputBuffer();
                        printf("��Ч������! ����������1-7��\n");
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
                        default: printf("��Ч��ѡ��! ����������1-7��\n");
                    }
                } while (algoChoice != 7);
                break;
            }
            case 2:
                runLargeScaleTests();
                break;
            case 3:
                printf("�����˳���\n");
                return 0;
            default:
                printf("��Ч��ѡ��! ����������1-3��\n");
        }
    }
    return 0;
}
