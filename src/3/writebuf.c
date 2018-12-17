#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUF_SIZE 1024
#define N 8

int main()
{
    int shmid = shmget(IPC_PRIVATE, BUF_SIZE, IPC_CREAT | 0666);
    int *addr = shmat(shmid, NULL, 0);
}