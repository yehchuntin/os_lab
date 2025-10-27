#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>


int main(){
    printf("程式即將變身...\n");
    printf("當前 PID: %d\n\n",getpid());

    // 方法 1：使用 execl（需要完整路徑）
    // execl("/bin/ls", "ls", "-l", "-h", NULL);

     // 方法 2：使用 execlp（自動搜索 PATH，推薦）
    execlp("ls","ls","-l","-h",NULL);
     //     ^程式名 ^argv[0] ^argv[1] ^結束標記
     // 註: 第一個參數是程式名，所以若用 execlp 或 execvp，前兩個會是相同的

    perror("exec 失敗\n");

    return 1;

}

// | 參數位置 | 名稱               | 說明                                  
// | ------- | -------------------| ----------------------------------- 
// | #1      |   path / file      | 作業系統要找的執行檔（像 `/bin/ls` 或 `python3`） (路徑)
// | #2      |   argv[0]          | 新程式自己看到的名字（通常和 #1 相同）               
// | #3~n    |   argv[1..n-1]     | 真正的命令列參數                            
// | 最後     |   NULL             | 結尾標記（必要）                            
