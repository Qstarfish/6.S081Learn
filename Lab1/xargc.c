#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"
int 
main(int argc,char* argv[]){
    char buf[512];
    char line[MAXARG];
    char * xargv[MAXARG];
    int xargc = 0;
    for(int i = 1; i < argc; i++){
        xargv[xargc++] = argv[i];
    }
    int n,lindex = 0;
    char * p = line;
    while((n=read(0,buf,sizeof(buf)))>0){
        for(int i = 0; i < n; i++){
            if(buf[i]=='\n'){
                line[lindex++] = 0;
                lindex = 0;
                xargv[xargc++] = p;
                p=line;
                xargv[xargc] = 0;
                xargc=argc-1;//reset xargc,让它存有argv命令行参数
                if(fork()==0){
                    exec(xargv[0],xargv);
                    exit(0);
                }
                wait(0);
            }else if(buf[i]==' '){
                line[lindex++] =0;
                xargv[xargc++] = p;
                p=&line[lindex];
            }else{
                line[lindex++] = buf[i];
            }
        }
    }
    exit(0);
}
