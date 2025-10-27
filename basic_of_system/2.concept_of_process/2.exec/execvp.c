#include <stdio.h>
#include <unistd.h>

int main() {
    // 使用 execv 系列：參數用數組
    char *args[] = {
        "python3",              // argv[0] 程式名
        "my_script.py",         // argv[1] 腳本檔案
        "--verbose",            // argv[2] 選項
        "--output", "result.txt", // argv[3], argv[4]
        NULL                    // 結束標記
    };
    
    printf("🐍 執行 Python 腳本...\n\n");
    
    execvp(args[0], args);  // 第一個參數是程式名
    
    perror("❌ 執行失敗");
    return 1;
}