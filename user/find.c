#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// ��ȡ�ļ���
char* fmtname(char *path) {
    static char buf[DIRSIZ+1];
    char *p;

    // �ҵ����һ��б�ܺ�ĵ�һ���ַ�
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = 0;
    return buf;
}

// �ж��Ƿ��ǵݹ�Ŀ¼��'.'��ʾ��ǰĿ¼��'..'��ʾ�ϼ�Ŀ¼
int isrecursion(char *path) {
    char *buf = fmtname(path);
    if(strcmp(buf, ".") == 0 || strcmp(buf, "..") == 0) {
        return 0;
    }
    return 1;
}


void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // ��Ŀ¼
    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // ��ȡ�ļ�״̬
    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // ����ļ�����Ŀ����ͬ����ӡ·��
    if(strcmp(fmtname(path), target) == 0) {
        printf("%s\n", path);
    }

    // �����ļ����ͽ��д���
    switch(st.type) {
    case T_FILE:// ������ļ�
        break;

    case T_DIR:// �����Ŀ¼
        // ���·�������Ƿ񳬳���������С
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';// ���б�ܣ���
        // ��ȡĿ¼��
        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0)// ���Ŀ¼��Ϊ�գ�����
                continue;
            memmove(p, de.name, DIRSIZ);// ��Ŀ¼����ļ���������buf��
            p[DIRSIZ] = 0;
            // ��ȡĿ¼��״̬
            if(stat(buf, &st) < 0) {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            // �ݹ������Ŀ¼
            if(isrecursion(buf)) {
                find(buf, target);
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    // ��������в���
    if(argc == 2) {
        find(".", argv[1]); // �ڵ�ǰĿ¼����Ŀ���ļ�
        exit(0);
    }
    if(argc == 3) {
        find(argv[1], argv[2]); // ��ָ��Ŀ¼����Ŀ���ļ�
        exit(0);
    } else {
        printf("usage: find [path] [target]\n"); // ��ӡ�÷���Ϣ
        exit(1);
    }
}
