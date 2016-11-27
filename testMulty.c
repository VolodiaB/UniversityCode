#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>

struct matrixargs {
    int N;
    int* MT1;
    int* MT2;
    int* res;
    int start;
    int end;
} args[120]; 

void *mythreadmulty (void *args1) {
    struct matrixargs *args = (struct matrixargs *) args1;
    int* M1 = args->MT1;
    int* M2 = args->MT2;
    int* res = args->res;
    int N = args->N;
    int start = args->start;
    int end = args->end;
    int i, j, l, m, k, tmp;
    for(i = start; i < end; i++) {
        m = i * N;
        for(j = 0; j < N; j++) {
            l = 0;
            for(k = 0; k < N; k++) {
                tmp += M1[m + k] * M2[j + l];
                l += N;
            }
            res[m +j] = tmp;
            tmp = 0;
        }
    }
    pthread_exit(NULL);
}


int main() {
    int i, N;
    scanf("%d", &N);
    int* MT1;
    int* MT2;
    int* res;
    int k;
    int ans;
    MT1 = malloc(sizeof(int) * N * N);
    MT2 = malloc(sizeof(int) * N * N);
    if(MT1 == NULL || MT2 == NULL) {
        printf("Cannot allocate memory\n");
        return -1;
    }
    for(i = 0; i < N * N; i++) 
        scanf("%d", &MT1[i]);
    for(i = 0; i < N * N; i++) 
        scanf("%d", &MT2[i]);                                    
    res = malloc(sizeof(int) * N * N);
    pthread_t threads[N];
    int pos = 0;
    for(k = 0; k < N; k++) {
        args[k].MT1 = MT1;
        args[k].MT2 = MT2;
        args[k].res = res;
        args[k].N = N;
        args[k].start = pos;
        pos++;
        args[k].end = (k == N - 1) ? N : pos;
        ans = pthread_create(&threads[k], NULL, mythreadmulty, (void *)&args[k]);
        if(ans) {
            printf("Thread creation failed\n");
            return -2;
        }
    }
    for(k = 0; k < N; k++)
        pthread_join(threads[k], NULL);
    printf("All\n");
    free(MT1);
    free(MT2);
    return 0;
}
