#include<stdio.h>
#include<stdlib.h>

int main(){
    FILE* fptr = fopen("example.txt","r");
    if(fptr == NULL){
        printf("Error opening file\n");
        return 1;
    }

    // Moving to the Beginning of the File
    fseek(fptr, 0, SEEK_SET); // 讀第一個字元

    char ch = fgetc(fptr);
    if(ch != EOF){
        printf("First character in the file: %c\n", ch);
    }

    fclose(fptr);
    return 0;
}