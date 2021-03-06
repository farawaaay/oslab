#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define MEM 75
#define SEM_KEY 76
#define BUF_SIZE 32
#define N 8

typedef struct Mem
{
    size_t total;
    char buf[N][BUF_SIZE];
} Mem;

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


Mem *m;

int main()
{
    int shmid = shmget(MEM, sizeof(Mem), IPC_CREAT | 0666);
    m = (Mem *)shmat(shmid, NULL, 0);
    int semid = semget(SEM_KEY, 2, IPC_CREAT | 0666);
    FILE *fp = fopen("./input.txt", "r");

    int i = 0;
    size_t total = -1;
    while (1)
    {
        P(semid, 0);
        if (total == -1)
        {
            fseek(fp, 0L, SEEK_END);
            total = m->total = ftell(fp);
            printf("%d", total);
            fseek(fp, 0L, SEEK_SET);
        }
        if (total < BUF_SIZE)
        {
            fread(m->buf[i = (i + 1) % N], total, 1, fp);
            total = 0;
        }
        else
        {
            fread(m->buf[i = (i + 1) % N], BUF_SIZE, 1, fp);
            total -= BUF_SIZE;
        }
        V(semid, 1);

        if (total == 0)
            break;
    }
}
