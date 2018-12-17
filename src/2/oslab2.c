#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/sem.h>

#define CALCULATE 0
#define PRINT 1

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int semid = 0;
int sum = 0;

void P(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}

void V(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}

void *subp1()
{
    for (int i = 1; i <= 100; i++)
    {
        P(semid, CALCULATE);
        sum += i;
        V(semid, PRINT);
    }
}

void *subp2()
{
    for (int i = 1; i <= 100; i++)
    {
        P(semid, PRINT);
        printf("The sum is: %d\n", sum);
        V(semid, CALCULATE);
    }
}

int main()
{
    semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);

    union semun arg1;
    arg1.val = 1;
    semctl(semid, CALCULATE, SETVAL, arg1);

    union semun arg2;
    arg2.val = 0;
    semctl(semid, PRINT, SETVAL, arg2);

    pthread_t threads[2];

    pthread_create(&threads[0], NULL, subp1, NULL);
    pthread_create(&threads[1], NULL, subp2, NULL);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    semctl(semid, 0, IPC_RMID);

    return 0;
}
