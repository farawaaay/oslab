#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <unistd.h>

#define BUF_SIZE 32
#define N 8
#define FIRST_KEY 100

// union semun {
//     int val;
//     struct semid_ds *buf;
//     unsigned short *array;
//     struct seminfo *__buf;
// };

typedef struct Mem
{
    size_t total;
    char buf[8][BUF_SIZE];
} Mem;
// 设缓冲区大小为N，队头out，队尾in，out、in均是下标表示:

// 初始时，in=out=0
// 队头队尾的更新用取模操作，out=(out+1)%N，in=(in+1)%N
// out==in表示缓冲区空，(in+1)%N==out表示缓冲区满
// 入队que[in]=value;in=(in+1)%N;
// 出队ret =que[out];out=(out+1)%N;
// 数据长度 len =( in - out + N) % N

Mem *addr;

int main()
{
    int shmid = shmget(IPC_PRIVATE, sizeof(Mem), IPC_CREAT | 0666);
    addr = (Mem *)shmat(shmid, NULL, 0);
    int semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);

    union semun arg1;
    arg1.val = N;
    semctl(semid, 0, SETVAL, arg1);

    union semun arg2;
    arg2.val = 0;
    semctl(semid, 1, SETVAL, arg2);

    pid_t p1;
    if ((p1 = fork()) == 0)
        execv("./readbuf", NULL);
    pid_t p2;
    if ((p2 = fork()) == 0)
        execv("./writebuf", NULL);

    int status;
    waitpid(p1, &status, 0);
    waitpid(p2, &status, 0);

    semctl(semid, 0, IPC_RMID);
    shmctl(shmid, IPC_RMID, 0);
}