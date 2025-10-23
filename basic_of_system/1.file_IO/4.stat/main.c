#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int main() {
    struct stat info; 
    struct stat folder;

    // 1️⃣ 呼叫 stat()
    if (stat("example.txt", &info) == 0) {

        // 2️⃣ 印出檔案大小
        printf("File size: %ld bytes\n", info.st_size);

        // 3️⃣ 印出修改時間
        printf("Last modified: %s", ctime(&info.st_mtime));

        // 4️⃣ 判斷是否為一般檔案
        if (S_ISREG(info.st_mode))
            printf("Type: Regular file\n");
        else if (S_ISDIR(info.st_mode))
            printf("Type: Directory\n");
        else
            printf("Type: Other\n");

    } else {
        perror("stat");
    }

    if(stat("example",&folder) == 0){
        if(S_ISDIR(folder.st_mode)){
            printf("Type: Directory");
        }else{
            printf("Not a directory.\n");
        }
    }else{
        perror("stat (example)");
    }

    return 0;
}
