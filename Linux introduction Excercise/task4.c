#include<stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
 if (argc < 2 ){
   printf("File wasn't provided!!\n");
   return -1;
}

FILE *fptr;
 fptr = fopen(argv[1],"r");
 if(fptr == NULL) {
  printf("Error!!\n");
  return -1;
 }
 char ch;
  while((ch = fgetc(fptr)) != EOF) {
    putchar(ch);
  }
 fclose(fptr);
 return 0;


}
