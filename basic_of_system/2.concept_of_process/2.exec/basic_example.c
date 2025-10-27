#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>


int main(){
    printf("程式即將變身...\n");
    printf("當前 PID: %d\n\n",getpid());

     // 方法 2：使用 execlp（自動搜索 PATH，推薦）
    execlp("ls","ls","-l","-h",NULL);
     //     ^程式名 ^argv[0] ^argv[1] ^結束標記
     // 註: 第一個參數是程式名，所以若用 execlp 或 execvp，前兩個會是相同的

    perror("exec 失敗\n");

    return 1;

}