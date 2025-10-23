#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("🎮 遊戲啟動器\n");
    
    char *games[] = {
        "gedit",      // 假裝是遊戲1
        "gnome-calculator",  // 假裝是遊戲2
        NULL
    };
    
    printf("選擇遊戲 (0 或 1): ");
    int choice;
    scanf("%d", &choice);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程：啟動選中的遊戲
        printf("🚀 啟動遊戲中...\n");
        execlp(games[choice], games[choice], NULL);
        
        printf("❌ 遊戲啟動失敗\n");
        exit(1);
    } else {
        // 父進程：等待遊戲結束
        printf("⏳ 遊戲運行中... (PID: %d)\n", pid);
        
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("✅ 遊戲正常退出\n");
        } else {
            printf("💥 遊戲崩潰了\n");
        }
    }
    
    return 0;
}