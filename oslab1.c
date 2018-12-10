#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <wait.h>
#define BUF_SIZE 100

pid_t pid_1, pid_2;
void sigint_handler(int sig)
{
    printf("\n"); // hao kan yi dian
    kill(pid_1, SIGUSR1);
    kill(pid_2, SIGUSR2);
}
void sigusr_handler(int sig)
{
    if (sig == SIGUSR1)
    {
        printf("Child Process 1 is Killed by Parent!\n");
        exit(0);
    }
    else if (sig == SIGUSR2)
    {
        printf("Child Process 2 is Killed by Parent!\n");
        exit(0);
    }
}

int main(void)
{
    int status;
    int fd[2];
    int count = 1;
    char buf[BUF_SIZE];

    signal(SIGINT, sigint_handler);

    if (pipe(fd) < 0)
    {
        perror("pipe failed!");
        exit(1);
    }

    if ((pid_1 = fork()) < 0)
    {
        perror("fork child process 1 failed!");
        exit(1);
    }
    else if (pid_1 == 0)
    {
        signal(SIGINT, SIG_IGN);
        signal(SIGUSR1, sigusr_handler);
        close(fd[0]);
        while (1)
        {
            sprintf(buf, "I send you %d times.\n", count);
            // printf("%s", buf);
            write(fd[1], buf, BUF_SIZE);
            sleep(1);
            count++;
        }
        exit(0);
    }
    if ((pid_2 = fork()) < 0)
    {
        perror("fork child process 2 failed!");
        exit(1);
    }
    else if (pid_2 == 0)
    {
        signal(SIGINT, SIG_IGN);
        signal(SIGUSR2, sigusr_handler);
        close(fd[1]);
        while (1)
        {
            read(fd[0], buf, BUF_SIZE);
            printf("%s", buf);
        }
    }

    waitpid(pid_1, &status, 0);
    waitpid(pid_2, &status, 0);

    printf("Parent Process is Killed!\n");

    return 0;
}