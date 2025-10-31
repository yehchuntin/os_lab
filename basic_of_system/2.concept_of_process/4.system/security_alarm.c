#include<stdio.h>
#include<stdlib.h>

int main(){
    char filename[100];

    printf("輸入要刪除的檔案名: ");
    scanf("%99s",filename);

    char command[200];
    sprintf(command, "rm %s", filename);
    system(command)

    return 0;
}