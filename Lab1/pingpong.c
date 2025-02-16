#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MSGSIZE 1

int main(int argc, char *argv[]) {
    int p1[2]; // 父进程写，子进程读
    int p2[2]; // 子进程写，父进程读

    // 创建两个管道
    if (pipe(p1) < 0 || pipe(p2) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid > 0) { // 父进程
        close(p1[0]); // 关闭 p1 的读端
        close(p2[1]); // 关闭 p2 的写端

        // 父进程向子进程发送消息
        if (write(p1[1], "a", MSGSIZE) != MSGSIZE) {
            fprintf(2, "write failed\n");
            exit(1);
        }
        close(p1[1]); // 关闭 p1 的写端
        
        wait(0);
        // 父进程等待子进程的回复
        char buf[MSGSIZE];
        if (read(p2[0], buf, MSGSIZE) != MSGSIZE) {
            fprintf(2, "read failed\n");
            exit(1);
        }
        close(p2[0]); // 关闭 p2 的读端

        printf("%d: received pong\n", getpid());
        exit(0);
    } else { // 子进程
        close(p1[1]); // 关闭 p1 的写端
        close(p2[0]); // 关闭 p2 的读端

        // 子进程读取父进程的消息
        char buf[MSGSIZE];
        if (read(p1[0], buf, MSGSIZE) != MSGSIZE) {
            fprintf(2, "read failed\n");
            exit(1);
        }
        close(p1[0]); // 关闭 p1 的读端

        printf("%d: received ping\n", getpid());

        // 子进程向父进程发送回复
        if (write(p2[1], buf, MSGSIZE) != MSGSIZE) {
            fprintf(2, "write failed\n");
            exit(1);
        }
        close(p2[1]); // 关闭 p2 的写端
    }
    return 0;
}
