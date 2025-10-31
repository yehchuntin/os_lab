#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
    printf("父進程 (PID:%d) 開始\n\n",getpid());

    pid_t pid = fork();

    if(pid == 0){
        // 子進程
        printf("子進程 (PID:%d) 工作中...\n",getpid());
        sleep(2); // 模擬工作
        printf("子進程完成工作\n");
        exit(42); // 返回退出碼 42
    }
    else{
        printf("父進程等待子進程 (PID:%d)...\n",pid);

        int status;
        pid_t finish_pid = wait(&status); // 要用指標接
        printf("\n子進程 %d 已結束\n",finish_pid);

        if(WIFEXITED(status)){   // 檢查是否正常退出（返回 true/false）
            int signal = WTERMSIG(status);
            printf("被信號 %d 終止\n",signal);
        }
    }

    return 0;
}


