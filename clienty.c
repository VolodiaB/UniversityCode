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
#define MAXLEN 256
int main() {
    struct mymsgbuf {
        long mtype;// add sem
        struct {
            int N;
            char answer;
            key_t shmkey;
            off_t offset;
            pid_t ID;
            float det;
        }info;
    }mybuf;
    int N;
    int len;
    int i;
    int *array, *array1;
    int shmid;
    long ID = getpid();
    key_t msgkey;
    char filename1[] = "servery.c";
    int msqid;
    int flag;
    //read number
    printf("Enter your number\n");
    scanf("%d", &N);
    if(N < 1) {
        printf("Sorry, number must be > 0\n");
        return -1;
    }
    array1 = malloc(sizeof(int) * (N * N));
    if (array1 == NULL) {
        printf("Allocation memory - %s\n", strerror(errno));
        return -71;
    }
    //generation msgkey
    msgkey = ftok(filename1, 2);
    if(msgkey < 0) {
        printf("Error generation key create %s\n", strerror(errno));
        return -2;
    }
    //msqconnection
    msqid = msgget(msgkey, 0);
    if(msqid < 0) {
        printf("Message queue failed %s\n", strerror(errno));
        return -3;
    }
    //send message
    mybuf.mtype = 3; // servertype
    mybuf.info.N = N;
    mybuf.info.answer = '0';
    mybuf.info.offset = 0;
    mybuf.info.shmkey = 0;
    mybuf.info.ID = ID;
    mybuf.info.det = -1.11;
    len = MAXLEN;
    flag = msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0);
    if(flag < 0) {
        printf("Message send - %s\n", strerror(errno));
        return -4;
    }
    sleep(1);// later waiting answer will be semaphore
    //get answer
    len = msgrcv(msqid, (struct msgbuf *) &mybuf, MAXLEN,ID, 0);
    if(len < 0) {
        printf("Message rcv - %s\n", strerror(errno));
        return -5;
    }
    if(mybuf.info.answer == 'N') {
        printf("Sorry, you do not guess number\n");
        return -6;
    }
    printf("Server answer is %c\n", mybuf.info.answer);
    // input matrix
    array = malloc(sizeof(int) * (N * N));
    if (array == NULL) {
        printf("Allocation memory - %s\n", strerror(errno));
        return -7;
    }
    // shm include

    shmid = shmget(mybuf.info.shmkey, (N * N ) * sizeof(int), 0);
    if(shmid < 0) {
        printf("Shmerror - %s\n", strerror(errno));
        return -8;
    }
    array = (int*)shmat(shmid, 0, 0) + mybuf.info.offset;// shmaddr!!!!!!
    if(array == (int*)(-1)) {
        printf("Cannot attach shm %s\n", strerror(errno));
        return -9;
    }
    printf("Input matrix\n");
    for(i = 0; i < N * N; i++) {
        scanf("%d", &array1[i]);
    }
    for(i = 0; i < N * N; i++) {
        array[i] = array1[i];
    }
    mybuf.info.answer = 'R';
    mybuf.mtype = 3;
    len = MAXLEN;
    flag = msgsnd(msqid, (struct msgbuf *) &mybuf, len, 0);
    if(flag < 0) {
        printf("Message send23 - %s\n", strerror(errno));
        return -12;
    }
    len = msgrcv(msqid, (struct msgbuf *) &mybuf, MAXLEN,ID, 0);// wait message determinant
    if(len < 0) {
        printf("Message rcv - %s\n", strerror(errno));
        return -10;
    }
    if (mybuf.info.det == -1.11) {
        printf("Detfail\n");
        return -11;
    }
    printf("Determinant of multy/secret/matrix = %f\n", mybuf.info.det);
    len = shmdt(array);
    if(len < 0) {
        printf("Cannot delete shm %s\n", strerror(errno));
        return -12;
    }
    return 0;
}
