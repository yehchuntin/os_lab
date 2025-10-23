#include<stdio.h>
#include<stdlib.h>


int main(){
    FILE *fpointer = fopen("employees.txt","a");
    // w 模式:會覆寫原本的資料
    // a 模式:會接在原本的資料後面


    fprintf(fpointer, "\nKelly, Customer Service");

    fclose(fpointer); // 像是把fpointer(檔案指標)free掉
    return 0;
}