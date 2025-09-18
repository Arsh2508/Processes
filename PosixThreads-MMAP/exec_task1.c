#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

const int loop = 3;

int main() 
{
    for(int i = 0; i < loop; ++i) {
        
        if(i == 0) {
            pid_t pid1 = fork();
    
            if(pid1 < 0) {
                perror("Fork failed!\n");
                return -1;
            }
            if(pid1 == 0) {
                execlp("ls", "ls", "-ls",  NULL);
                
                perror("Exec failed!\n");
                exit(1);
            }
            else {
                wait(NULL);
            }
        }

        else if(i == 1) {
            pid_t pid2 = fork();

            if(pid2 < 0) {
                perror("Fork failed");
                exit(1);
            }
            
            if(pid2 == 0) {
                execlp("whoami", "whoami", NULL, NULL);
                
                perror("exec failed!\n");
                exit(1);
            }
            else {
                wait(NULL);
            }
        }

        else if(i == 2) {
            pid_t pid3 = fork();

            if(pid3 < 0) {
                perror("Fork failed!\n");
                exit(1);
            }
            if(pid3 == 0) {
                execlp("date", "date", NULL, NULL);
                
                perror("exec failed!\n");
                exit(1);
            }
            else {
                wait(NULL);
            }
        }

    }
    return 0;
}