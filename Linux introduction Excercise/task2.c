#include <stdio.h>
#include <stdlib.h>
int main(){

 FILE *fptr;
 
 fptr = fopen("task2.txt", "r");
 if(fptr == NULL) {
   printf("Error!!\n");
   return -1;
}
 char ch;   
    while ((ch = fgetc(fptr)) != EOF) {
        putchar(ch);
    }

    // Close the file
    fclose(fptr);
}
