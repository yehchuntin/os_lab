#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("🏭 工廠啟動，準備生產 3 個產品\n\n");
    
    // 創建 3 個工人（子進程）
    for (int i = 1; i <= 3; i++) {
        pid_t pid = fork();
        
        if (pid == 0) {
            // 子進程：工人開始工作
            printf("👷 工人 %d (PID:%d) 開始生產產品\n", i, getpid());
            sleep(3-i);  // 模擬工作時間 
            printf("✅ 工人 %d 完成產品！\n", i);
            return i;  // 返回產品編號
        }
    }
    
    // 父進程：老闆等待所有工人完成
    printf("👔 老闆等待所有工人完成...\n\n");
    
    for (int i = 0; i < 3; i++) {
        int status;
        pid_t finished_pid = wait(&status);  // 等待任意子進程
        
        if (WIFEXITED(status)) {
            int product_id = WEXITSTATUS(status);
            printf("📦 收到產品 %d (來自 PID:%d)\n", product_id, finished_pid);
        }
    }
    
    printf("\n✨ 所有產品完成！工廠關閉\n");
    return 0;
}

// 有可能的問題
// 1. 為什麼不是「工人1~3都先印」，老闆才印「等待」？

// 作業系統排程會決定誰先跑：父或子都可能先拿到 CPU。也就是：
// 有時候父行程會先印出「老闆等待…」，然後子行程才輪到印「工人開始生產…」；
// 也可能部分子行程先印，再輪到父行程。
// 這種輸出交錯就是多行程同時執行的自然現象；沒有誰「一定先」。
// 若你要「確定」老闆在工人之後才印，就需要同步機制（例如用 pipe/信號量/共享記憶體讓子行程先回報「我開始了」），父行程等收到 3 份「開始」訊號後再印「老闆等待…」。

// 2. 為什麼 WEXITSTATUS(status) 剛好等於 1、2、3？

// 因為子行程裡 return i;
// 在 main() 裡 return <值> 會等價於 exit(<值>)，這個值就成了子行程的退出碼 (exit code)。
// 注意：exit code 通常只有 8-bit（0–255） 有效，再大的值會被截斷。