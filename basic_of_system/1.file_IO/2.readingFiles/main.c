#include<stdio.h>
#include<stdlib.h>

int main(){
    char line[255];
    FILE *fpointer = fopen("employees.txt","r");

    fgets(line, sizeof(line),fpointer); // 讀取檔案第一行，存進line
    fgets(line, sizeof(line),fpointer); // 讀取檔案第二行，存進line
    printf("%s",line);

    fclose(fpointer);
    return 0;
}