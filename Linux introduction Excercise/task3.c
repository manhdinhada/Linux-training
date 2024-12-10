#include<stdio.h>

int main(){
 int input_ID;
 char path[20];
 char name_proc[20];


 printf("Enter a process ID: "); 
 scanf("%d",&input_ID);
 FILE *fptr;
 snprintf(path, sizeof(path),"/proc/%d/comm",input_ID);
 fptr = fopen(path,"r");
 if(fptr == NULL) {
    printf("Error opening proc file\n");
    return -1;
 }
 char ch;
 printf("The name of the process with PID :\n");
 while((ch = fgetc(fptr)) != EOF) {
    putchar(ch);
 }
 fclose(fptr);


}

