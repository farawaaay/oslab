#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define BUF_SIZE 32
#define N 8
#define KEY 75

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
// 设缓冲区大小为N，队头out，队尾in，out、in均是下标表示:

// 初始时，in=out=0
// 队头队尾的更新用取模操作，out=(out+1)%N，in=(in+1)%N
// out==in表示缓冲区空，(in+1)%N==out表示缓冲区满
// 入队que[in]=value;in=(in+1)%N;
// 出队ret =que[out];out=(out+1)%N;
// 数据长度 len =( in - out + N) % N

Mem *m;

int main()
{
    int shmid = shmget(76, sizeof(Mem), IPC_CREAT | 0666);
    m = (Mem *)shmat(shmid, NULL, 0);
    int semid = semget(75, 2, IPC_CREAT | 0666);
    FILE *fp = fopen("./input.txt", "r");

    int i = 0;
    size_t total = -1;
    while (1)
    {
        printf("P1\n");
        P(semid, 0);
        printf("P2\n");
        if (total == -1)
        {
            printf("ok");
            fseek(fp, 0L, SEEK_END);
            printf("ok");
            total = m->total = ftell(fp);
            printf("%d", total);
            fseek(fp, 0L, SEEK_SET);
        }
        printf("ok2");

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
        printf("V\n");
        V(semid, 1);

        if (total == 0)
            break;
    }
}