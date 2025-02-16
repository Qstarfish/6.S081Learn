#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MSGSIZE 36
#define ONE '1'
#define ZERO '0'

// 筛选素数的函数
void sieve(int pipe_fd[]) {
    int prime;
    // 从管道中读取第一个数作为当前的素数
    read(pipe_fd[0], &prime, sizeof(prime));
    if (prime == 0) {// 0作为结束标记，如果读取到的数为0，则退出
        close(pipe_fd[0]);
        exit(0);
    }

    printf("prime %d\n", prime);

    int new_pipe_fd[2];
    pipe(new_pipe_fd);

    int pid = fork();
    if (pid > 0) { // 父进程
        close(new_pipe_fd[0]);// 关闭新管道的读端
        int num;
        // 从旧管道中读取数，如果不是当前素数的倍数，则写入新管道
        while (read(pipe_fd[0], &num, sizeof(num)) > 0) {
            if (num % prime != 0) {
                write(new_pipe_fd[1], &num, sizeof(num));
            }
        }
        close(new_pipe_fd[1]);
        close(pipe_fd[0]);
        wait(0);
    } else { // 子进程
        close(pipe_fd[0]);
        close(new_pipe_fd[1]);
        sieve(new_pipe_fd);
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    int pipe_fd[2];
    if (pipe(pipe_fd) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid > 0) { // 父进程，写入数据
        close(pipe_fd[0]);
        for (int i = 2; i < MSGSIZE; i++) {
            write(pipe_fd[1], &i, sizeof(i));
        }
        int end = 0;
        write(pipe_fd[1], &end, sizeof(end)); // 写入结束标志
        close(pipe_fd[1]);
        wait(0);
        exit(0); // 父进程退出
    } else { // 子进程，筛选素数
        close(pipe_fd[1]);
        sieve(pipe_fd);
        exit(0); // 子进程退出
    }
}
