#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 获取文件名
char* fmtname(char *path) {
    static char buf[DIRSIZ+1];
    char *p;

    // 找到最后一个斜杠后的第一个字符
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    memmove(buf, p, strlen(p));
    buf[strlen(p)] = 0;
    return buf;
}

// 判断是否是递归目录，'.'表示当前目录，'..'表示上级目录
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

    // 打开目录
    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取文件状态
    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 如果文件名与目标相同，打印路径
    if(strcmp(fmtname(path), target) == 0) {
        printf("%s\n", path);
    }

    // 根据文件类型进行处理
    switch(st.type) {
    case T_FILE:// 如果是文件
        break;

    case T_DIR:// 如果是目录
        // 检查路径长度是否超出缓冲区大小
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';// 添加斜杠！！
        // 读取目录项
        while(read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0)// 如果目录项为空，跳过
                continue;
            memmove(p, de.name, DIRSIZ);// 将目录项的文件名拷贝到buf中
            p[DIRSIZ] = 0;
            // 获取目录项状态
            if(stat(buf, &st) < 0) {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            // 递归查找子目录
            if(isrecursion(buf)) {
                find(buf, target);
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    // 检查命令行参数
    if(argc == 2) {
        find(".", argv[1]); // 在当前目录查找目标文件
        exit(0);
    }
    if(argc == 3) {
        find(argv[1], argv[2]); // 在指定目录查找目标文件
        exit(0);
    } else {
        printf("usage: find [path] [target]\n"); // 打印用法信息
        exit(1);
    }
}
