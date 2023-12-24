#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main (){
    int s;
    s= 15/0;
    printf("hello world\n");
    return 1;
}