//
// Created by volodia on 09.11.16.
//
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<sys/msg.h>
#include<pthread.h>
#include "matrix.h"
#include "mymath.h"


int main() {
    FILE *f;
    int N = 3;
    int i, j;
    pthread_t threads[N];
    int k, pos = 0;
    int **secret;
    int **array1;
    int *array;
    int offset;
    array1 = malloc(sizeof(int *) * N);
    for (i = 0; i < N; i++) {
        array1[i] = malloc(sizeof(int) * N);
        for (j = 0; j < N; j++)
            array1[i][j] = 0;
    }
    int len, len1;
    int flag;
    int det, ans;
    int **res;
    res = malloc(sizeof(int *) * N);
    for (i = 0; i < N; i++) {
        res[i] = malloc(sizeof(int) * N);
        for (j = 0; j < N; j++)
            res[i][j] = 0;
    }
    int* resstr;
    resstr = malloc(sizeof(int) * N * N);
    buildstring(res, N, resstr);
    key_t sharekey, msgkey;
    char filename1[] = "servery.c";
    char filename2[] = "clienty.c";
    int msqid, shmid;
    struct mymsgbuf {
        long mtype;// add sem
        struct {
            int N;
            char answer;
            key_t shmkey;
            off_t offset;
            pid_t ID;
            float det;
        } info;
    } mybuf;
    long smas[3][2];
    int count = 0;
    int *array4;
    int *secretstr;
    secretstr = malloc(sizeof(int) * N * N);
    buildstring(secret, N, secretstr);
    array4 = malloc(sizeof(int *) * N * N);
    //open file and get secret matrix
    f = fopen("matrix", "r");
    if (f == NULL) {
        printf("Cannot open file\n");
        return -1;
    }
    secret = malloc(sizeof(int *) * N);
    for (i = 0; i < N; i++) {
        secret[i] = malloc(sizeof(int) * N);
        for (j = 0; j < N; j++)
            fscanf(f, "%d", &secret[i][j]);
    }
    //talken generation
    msgkey = ftok(filename1, 2);
    if (msgkey < 0) {
        printf("Error generation key create %s\n", strerror(errno));
        return -2;
    }
    sharekey = ftok(filename2, 2);
    if (sharekey < 0) {
        printf("Error generation key create %s\n", strerror(errno));
        return -3;
    }
    //sharememory creation for 3 clients
    //i don't want to do more
    msqid = msgget(msgkey, IPC_CREAT | 0666);// 3 server type
    if (msqid < 0) {
        if (errno == EEXIST) {
            msqid = msgget(msgkey, 0);
            if (msqid < 0) {
                printf("Message queue failed %s\n", strerror(errno));
                return -4;
            }
        }
        printf("Message queue failed1 %s\n", strerror(errno));
        return -4;
    }
    shmid = shmget(sharekey, (N * N) * sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        if (errno == EEXIST) {
            shmid = shmget(sharekey, (N * N) * sizeof(int), 0);
            if (shmid < 0) {
                printf("Sharemem creat failed %s\n", strerror(errno));
                return -5;
            }
        }
        printf("Sharemem creat failed1 %s\n", strerror(errno));
        return -5;
    }
    array = malloc(sizeof(int) * (N * N) * 3);// 3 clients
    if (array == NULL) {
        printf("Allocation memory - %s\n", strerror(errno));
        return -7;
    }
    array = (int *) shmat(shmid, 0, 0);
    //enter to cycle
    while (1) {
        //waiting messages
        len1 = msgrcv(msqid, (struct msgbuf *) &mybuf, MAXLEN, 3, 0);
        if (len1 < 0) {
            printf("Error server - %s", strerror(errno));
            return -11;
        }
        if (mybuf.info.N == N) {
                len = MAXLEN;
                mybuf.mtype = mybuf.info.ID;
                mybuf.info.N = N;
                mybuf.info.answer = 'Y';
                mybuf.info.offset = (N * N) * count * 4; // kak offset???
                mybuf.info.shmkey = sharekey;
                mybuf.info.ID = mybuf.info.ID;
                mybuf.info.det = -1.11;
                smas[count][0] = count;
                smas[count][1] = mybuf.info.ID;
                count++;
                flag = msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0);
                if (flag < 0) {
                    printf("Message send1 - %s\n", strerror(errno));
                    return -7;
                }
            } else {
                len = MAXLEN;
                mybuf.mtype = mybuf.info.ID;
                mybuf.info.answer = 'N';
                mybuf.info.offset = 0; // kak offset???
                mybuf.info.shmkey = 0;
                mybuf.info.ID = mybuf.info.ID;
                mybuf.info.det = -1.11;
                flag = msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0);
                if (flag < 0) {
                    printf("Message send - %s\n", strerror(errno));
                    return -8;
                }
            }
        len1 = msgrcv(msqid, (struct msgbuf *) &mybuf, MAXLEN, 3, 0);
        if(len1 < 0) {
            printf("Error server12 - %s", strerror(errno));
            return -11;
        }
        if(mybuf.info.answer == 'R') {
            offset = mybuf.info.offset;
            for (j = offset; j < offset + N * N; j++) {
                array4[j - offset] = array[j];
                printf("%d ", array4[j - offset]);
            }
            printf("\n");
            //multiply
            buildmatrix(array4, N, array1);
            PrintM(array1, N);
            buildstring(array1, N, array4);
            for(k = 0; k < N; k++) {
               args[k].MT1 = array4;
               args[k].MT2 = secretstr;
               args[k].res = resstr;
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
           buildmatrix(resstr, N, res);
           PrintM(res, N);
           //res = Multy(array1, secret, N);

           //det = Det(res, N);
           printf("%d\n", det);

            mybuf.mtype = mybuf.info.ID;
            mybuf.info.answer = 'Y';
            mybuf.info.offset = 0;
            mybuf.info.shmkey = 0;
            mybuf.info.ID = smas[i][1];
            mybuf.info.det = det;
            flag = msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0);
            if (flag < 0) {
                printf("Message send - %s\n", strerror(errno));
                return -9;
            }
        }
    }
    return 0;
}
