#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
    pid_t pid = fork();

    if(pid == 0){
        printf("子進程工作中...\n");
        sleep(3);
        exit(0);
    }
    else{
        // 非阻塞檢查子進程狀態
        printf("👔 父進程繼續工作...\n");

        int status;
        pid_t result;

        for(int i = 0; i < 5; i++){
            // WNOHANG(控制等待行為) : 非阻塞，立即返回 (Wait No Hang)
            result = waitpid(pid, &status, WNOHANG);

            if(result == 0){
                printf("=== 檢查 %d: 子進程還在工作 ===\n",i+1);
                sleep(1);
            }
            else{
                printf("=== 子進程完成! ===\n");
                break;
            }
        }
        if(result == 0){
            printf("    阻塞等待子進程...\n");
            waitpid(pid, &status, 0); // 阻塞等待
        }
    }
    return 0;
}

// | 情境                                   | 對應系統呼叫                         
// | -------------------------------------- | ------------------------------ 
// | 你等朋友寫完報告才能離開 → 你被卡住       | `wait()`                       
// | 你只等「某個」朋友（例如小明）寫完報告才走 | `waitpid(pid小明, ..., 0)`       
// | 你問一聲「寫完沒？」沒寫完就先去做別的事   | `waitpid(pid小明, ..., WNOHANG)` 
