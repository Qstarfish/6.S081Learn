#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MSGSIZE 1

int main(int argc, char *argv[]) {
    int p1[2]; // ������д���ӽ��̶�
    int p2[2]; // �ӽ���д�������̶�

    // ���������ܵ�
    if (pipe(p1) < 0 || pipe(p2) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid > 0) { // ������
        close(p1[0]); // �ر� p1 �Ķ���
        close(p2[1]); // �ر� p2 ��д��

        // ���������ӽ��̷�����Ϣ
        if (write(p1[1], "a", MSGSIZE) != MSGSIZE) {
            fprintf(2, "write failed\n");
            exit(1);
        }
        close(p1[1]); // �ر� p1 ��д��
        
        wait(0);
        // �����̵ȴ��ӽ��̵Ļظ�
        char buf[MSGSIZE];
        if (read(p2[0], buf, MSGSIZE) != MSGSIZE) {
            fprintf(2, "read failed\n");
            exit(1);
        }
        close(p2[0]); // �ر� p2 �Ķ���

        printf("%d: received pong\n", getpid());
        exit(0);
    } else { // �ӽ���
        close(p1[1]); // �ر� p1 ��д��
        close(p2[0]); // �ر� p2 �Ķ���

        // �ӽ��̶�ȡ�����̵���Ϣ
        char buf[MSGSIZE];
        if (read(p1[0], buf, MSGSIZE) != MSGSIZE) {
            fprintf(2, "read failed\n");
            exit(1);
        }
        close(p1[0]); // �ر� p1 �Ķ���

        printf("%d: received ping\n", getpid());

        // �ӽ����򸸽��̷��ͻظ�
        if (write(p2[1], buf, MSGSIZE) != MSGSIZE) {
            fprintf(2, "write failed\n");
            exit(1);
        }
        close(p2[1]); // �ر� p2 ��д��
    }
    return 0;
}
