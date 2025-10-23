#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid;
    int count = 0;  // 測試變數
    
    printf("===== 程式開始 =====\n");
    printf("原始進程 PID: %d\n\n", getpid());
    
    // 創建子進程
    pid = fork();
    
    if (pid < 0) {
        // fork 失敗
        perror("fork 失敗");
        return 1;
        
    } else if (pid == 0) {
        // 這裡是子進程執行的代碼
        printf("👶 [子進程]\n");
        printf("   我的 PID: %d\n", getpid());
        printf("   我的父進程 PID: %d\n", getppid());
        printf("   fork() 返回值: %d\n", pid);
        
        // 修改變數（不會影響父進程）
        count = 100;
        printf("   我的 count: %d\n\n", count);
        
    } else {
        // 這裡是父進程執行的代碼
        printf("👨 [父進程]\n");
        printf("   我的 PID: %d\n", getpid());
        printf("   子進程 PID: %d\n", pid);
        printf("   fork() 返回值: %d\n", pid);
        printf("   我的 count: %d\n\n", count);
    }
    
    // 父子進程都會執行這段
    printf("進程 %d 說：再見！\n", getpid());
    
    return 0;
}