#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#define MAX_N 320000  // �����Ʒ����
typedef struct {
    int weight;
    double value;  
} Item;

Item items[MAX_N];
int n;
int capacity;
double max_value; 

// ��̬�滮����⣨��Դ��ģ�����Ż���
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

// ���������Ʒ����
void generateRandomItems() {
    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        items[i].weight = 1 + rand() % 100;              // ����1-100
        items[i].value = 100 + (rand() % 900) + (rand() % 100) / 100.0; // ��ֵ100-999.99
    }
}

// ���Դ��ģ���ݼ�
void runLargeScaleTests() {
    int test_n[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 
                   10000, 20000, 40000, 80000, 160000, 320000};
    int test_capacity[] = {10000, 100000, 1000000};
    int num_n = sizeof(test_n)/sizeof(test_n[0]);
    int num_cap = sizeof(test_capacity)/sizeof(test_capacity[0]);

    // ��CSV�ļ�׼��д��
    FILE *csv_file = fopen("��̬�滮_���ģ���ݲ��Խ��.csv", "w");
    if (csv_file == NULL) {
        printf("�޷�����CSV�ļ�!\n");
        return;
    }
    
    // д��CSV�ļ�ͷ
    fprintf(csv_file, "��Ʒ����,��������,����ֵ,����ʱ��(s)\n");

    printf("\n���ģ���Կ�ʼ...\n");
    printf("| %-10s | %-10s | %-15s | %-15s |\n", "��Ʒ����", "��������", "����ֵ", "����ʱ��(s)");
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
            
            // ���������̨
            printf("| %-10d | %-10d | %-15.2f | %-15.3f |\n", 
                  n, capacity, max_value, time_used);
            
            // д��CSV�ļ�
            fprintf(csv_file, "%d,%d,%.2f,%.3f\n", 
                   n, capacity, max_value, time_used);
        }
    }
    
    fclose(csv_file);
    printf("\n���Խ���ѱ��浽 ��̬�滮_���ģ���ݲ��Խ��.csv �ļ���\n");
}

int main() {
    printf("��̬�滮�Ĵ��ģ���ԣ�\n");
    runLargeScaleTests();
    return 0;
}
