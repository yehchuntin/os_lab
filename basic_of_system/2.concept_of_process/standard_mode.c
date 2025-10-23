#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子進程：執行 ls 命令
        printf("子進程開始執行 ls\n");
        execlp("ls", "ls", "-l", NULL);
        
        // 如果 exec 失敗才會執行到這裡
        perror("exec 失敗");
        return 1;
    } else {
        // 父進程：等待子進程完成
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("子進程結束，返回值: %d\n", WEXITSTATUS(status));
        }
    }
    
    return 0;
}