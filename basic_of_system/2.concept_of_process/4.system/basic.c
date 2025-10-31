#include<stdlib.h>
#include<stdio.h>

int main(){
    printf("=== 使用 system() 執行命令 ===\n\n");

    // Example 1 : 列出檔案
    printf("當前目錄內容: \n");
    system("ls -lh");

    // Example 2 : 查看系統資訊
    printf("系統資訊: \n");
    system("uname -a");

    // Example 3 :　執行複雜指令(管道)
    printf("最大的 5 個檔案:\n");
    system("ls -lh | sort -k5 -hr | head -5");

    return 0;
}