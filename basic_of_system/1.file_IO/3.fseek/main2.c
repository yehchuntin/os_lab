#include<stdio.h>
#include<stdlib.h>

int main(){
    FILE* fptr = fopen("example.txt","r");
    if(fptr == NULL){
        printf("Error opening file\n");
        return 1;
    }

    // Moving to the Beginning of the File
    fseek(fptr, 7, SEEK_SET); // Move to the 10th byte from the beginning
    // fseek(fptr, -3, SEEK_CUR); // Move 3 bytes backward from the current position

    char ch = fgetc(fptr);
    if(ch != EOF){
        printf("First character in the file: %c\n", ch);
    }

    fclose(fptr);
    return 0;
}