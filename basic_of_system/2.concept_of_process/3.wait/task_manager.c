#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<time.h>

#define NUM_TASKS 5

int main(){
    printf("Task Manager starts\n");
    printf("準備執行 %d 個任務\n\n",NUM_TASKS);

    pid_t children[NUM_TASKS];
    time_t start_time = time(NULL);

    for(int i = 0; i < NUM_TASKS; i++){
        children[i] = fork();

        if(children[i] == 0){
            // 子行程：先為本行程獨立設亂數種子
            unsigned seed = (unsigned)(time(NULL) ^ getpid() ^ (i+1)*0x9e3779b9u);
            srand(seed);
            
            // 子進程 : 執行任務
            printf(" 任務 %d 開始 (PID:%d)\n",i+1, getpid());

            // 模擬不同的工作時間
            int work_time = (rand() % 3) + 1;
            sleep(work_time);

            // 隨機成功或失敗
            int success = (rand() % 3) != 0; // 75% 成功率

            if(success){
                printf("任務 %d 完成\n",i+1);
                exit(0); // 成功
            }
            else{
                printf("任務 %d 失敗\n",i+1);
                exit(1); // 失敗
            }
        }
    }

    // 父進程：等待所有任務完成
    printf("\n等待所有任務完成\n\n");

    int success_count = 0;
    int failed_count = 0;

    for(int i = 0; i < NUM_TASKS; i++){
        int status;
        pid_t finished = wait(&status); // 等待任意子進程
        
        if(WIFEXITED(status)){
            int exit_code = WEXITSTATUS(status);
            if(exit_code == 0){
                success_count++;
            }
            else{
                failed_count++;
            }
        }
    }

    time_t end_time = time(NULL);

    // 顯示統計
    printf("\n" "═══════════════════════\n");
    printf("📊 任務統計報告\n");
    printf("═══════════════════════\n");
    printf("總任務數: %d\n", NUM_TASKS);
    printf("✅ 成功: %d\n", success_count);
    printf("❌ 失敗: %d\n", failed_count);
    printf("⏱️  總耗時: %ld 秒\n", end_time - start_time);
    printf("═══════════════════════\n");
    
    return 0;

}