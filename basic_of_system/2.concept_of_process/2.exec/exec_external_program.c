#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(){
    printf("=== 照片處理程式 ===\n\n");

    char *input = "photo.jpg";
    char *output = "photo_resized.jpg";

    printf("📸 開始處理照片: %s\n", input);
    printf("🎯 輸出檔案: %s\n\n", output);

    execlp("convert",
           "convert",
            input,
           "-resize", "50%",
           "output",
            NULL);

    // 如果執行到這裡，說明 exec 失敗了
    perror("❌ 無法執行 convert 命令");
    printf("💡 請確認已安裝 ImageMagick\n");
    return 1;


}