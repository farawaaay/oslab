#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <unistd.h>

#define MEM 75
#define SEM_KEY 76
#define BUF_SIZE 32
#define N 8

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

typedef struct Mem
{
    size_t total;
    char buf[N][BUF_SIZE];
} Mem;


Mem *addr;

int main()
{
    int shmid = shmget(MEM, sizeof(Mem), IPC_CREAT | 0666);
    addr = (Mem *)shmat(shmid, NULL, 0);
    int semid = semget(SEM_KEY, 2, IPC_CREAT | 0666);

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
