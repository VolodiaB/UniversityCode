//
// Created by volodia on 04.11.16.
//
//#include<stdio.h>
//#include<stdlib.h>
#ifdef MY_MATH_H
#define MY_MATH_H

#include "matrix.h"

static void *mythreadmulty (void *args1) {
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


void PrintM (int **array, int N) {
    int i, j;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++)
            printf("%d ", array[i][j]);
        printf("\n");
    }
}

void GetM (int **array, int **result, int i, int j, int N) {
    int iq, jq, id = 0, jd;
    for(iq = 0; iq < N -1; iq++) {
        if(iq == i)
            id = 1;
        jd = 0;
        for(jq = 0; jq < N - 1; jq++) {
            if (jq == j)
                jd = 1;
            result[iq][jq] = array[iq + id][jq + jd];
        }
    }
}
// need offset
void buildmatrix(int* array, int N, int** array1) {
    int i = 0, j = 0, k = 0;
    int *array2;
    array2 = malloc(sizeof(int) * N * N);
    for(i = 0; i < N * N; i++)
        array2[i] = array[i]; // затираем элемент 10 флаг
    for(i = 0; i < N; i++)
        for(j = 0; j < N; j++) {
            array1[i][j] = array2[k];
            k++;
        }
}

void buildstring(int** matrix, int N, int* array) {
    int i, j;
    for(i = 0; i < N; i++) {
        for(j = 0; i < N; i++) {
            array[i * N + j] = matrix[i][j];
        }
    }
}

int Det(int **array, int N) {
    int i, det = 0, k = 1, n = N - 1;
    int **str;
    str = malloc(N * sizeof(int*));
    for(i = 0; i < N; i++)
        str[i] = malloc(N * sizeof(int));
    if (N < 1)
        printf("What are you doing?\n");
    if (N == 1) {
        det = array[0][0];
        return det;
    }
    if (N == 2) {
        det = (array[0][0] * array[1][1]) - (array[0][1] * array[1][0]);
        return det;
    }
    if (N > 2) {
        for(i = 0; i < N; i++) {
            GetM(array, str, i, 0, N);
            det = det + k * array[i][0] * Det(str, n);
            k = -k;
        }
    }
    return(det);
}

int **Multy(int **array1, int **array2, int N) {
    int **res;
    int i, j, k;
    res = malloc(N * sizeof(int*));
    for(i = 0; i < N; i++)
        res[i] = malloc(N * sizeof(int));
    for(i = 0; i < N; i++)
        for(j = 0; j < N; j++) {
            res[i][j] = 0;
            for(k = 0; k < N; k++)
                res[i][j] = res[i][j] + array1[i][k]*array2[k][j];
        }
    return res;
}

#endif
