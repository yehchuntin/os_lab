#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>

int main(){
    pid_t pid;  // 儲存 fork() 的回傳值（代表子行程的 ID）
    
    printf("Start, PID = %d\n",getpid()); // 印出目前主程式的 PID（父行程的 ID）

    pid = fork();  // 建立一個子行程（複製目前的程式）

    if(pid < 0){ // 建立失敗（例如資源不足）
        perror("fork failed\n");
        return 1;
    }

    else if(pid == 0){ // 現在這段程式是在 子行程 執行
        printf("I am Child Process, PID = %d, Father Process = %d\n",getpid(), getppid());
    }
    else{
        printf("I am Father Process, PID = %d, Child Process = %d\n",getpid(),pid);
    }

    printf("Process %d executed completely\n",getpid());
    return 0;
}