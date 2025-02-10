#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MSGSIZE 36
#define ONE '1'
#define ZERO '0'

// ɸѡ�����ĺ���
void sieve(int pipe_fd[]) {
    int prime;
    // �ӹܵ��ж�ȡ��һ������Ϊ��ǰ������
    read(pipe_fd[0], &prime, sizeof(prime));
    if (prime == 0) {// 0��Ϊ������ǣ������ȡ������Ϊ0�����˳�
        close(pipe_fd[0]);
        exit(0);
    }

    printf("prime %d\n", prime);

    int new_pipe_fd[2];
    pipe(new_pipe_fd);

    int pid = fork();
    if (pid > 0) { // ������
        close(new_pipe_fd[0]);// �ر��¹ܵ��Ķ���
        int num;
        // �Ӿɹܵ��ж�ȡ����������ǵ�ǰ�����ı�������д���¹ܵ�
        while (read(pipe_fd[0], &num, sizeof(num)) > 0) {
            if (num % prime != 0) {
                write(new_pipe_fd[1], &num, sizeof(num));
            }
        }
        close(new_pipe_fd[1]);
        close(pipe_fd[0]);
        wait(0);
    } else { // �ӽ���
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

    if (pid > 0) { // �����̣�д������
        close(pipe_fd[0]);
        for (int i = 2; i < MSGSIZE; i++) {
            write(pipe_fd[1], &i, sizeof(i));
        }
        int end = 0;
        write(pipe_fd[1], &end, sizeof(end)); // д�������־
        close(pipe_fd[1]);
        wait(0);
        exit(0); // �������˳�
    } else { // �ӽ��̣�ɸѡ����
        close(pipe_fd[1]);
        sieve(pipe_fd);
        exit(0); // �ӽ����˳�
    }
}
