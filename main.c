#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void extractFileName(const char fullFileName[256], char fileName[256]) {
    char *dot = strrchr(fullFileName, '.'); // Find the last dot in the string
    if (dot != NULL) {
        int dotPosition = dot - fullFileName; // Calculate the position of the last dot
        strncpy(fileName, fullFileName, dotPosition);
        fileName[dotPosition] = '\0'; // Null-terminate the file name
    } else {
        strcpy(fileName, fullFileName);
    }
}

void compile(char *prog) {
    if (execlp("gcc", "gcc", "-c", prog, NULL) == -1) {
        printf("Erreur de compilation");
        exit(EXIT_FAILURE);
    }
}

void link_edit(char *prog) {
    char progName[256];
    extractFileName(prog,progName);
    execlp("gcc", "gcc", prog, "-o", progName, NULL);
    printf("Erreur d'édition de liens");
    exit(EXIT_FAILURE);
    
}

void execute(char *prog) {
    char progName[256];
    extractFileName(prog,progName);
    char executeName[256]= "./";
    strcat(executeName, progName);
    if (execlp(executeName,executeName, NULL) == -1) {
        printf("Erreur d'exécution");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    int pid;
    int status;
    char progName[256];
    if (argc != 2) {
        printf("Usage: %s fichier_source.c\n", argv[0]);
        return -1;
    }
    extractFileName(argv[1],progName);
    pid = fork();
    if (pid == 0) { // P1
        printf("le pid de p1 est: %d, et son pere est: %d\n",getpid(), getppid());
        compile(argv[1]);
    } else if (pid > 0) { // Parent
        wait(&status);
        if (WIFEXITED(status)) { // Verifier la normal termination
            pid = fork();
            if (pid == 0) { // P2

                printf("le pid de p2 est: %d, et son pere est: %d\n",getpid(), getppid());
                strcat(progName, ".o");
                link_edit(progName);
            } else if (pid > 0) { // Parent
                printf("le pid est: %d\n",pid);
                wait(&status);
                if (WIFEXITED(status)) { // Check normal terminaison
                    pid = fork();
                    if (pid == 0) { // P3
                        printf("le pid de p3 est: %d, et son pere est: %d\n",getpid(), getppid());
                        execute(progName);
                    } else if (pid > 0) { // Parent
                        wait(&status);
                        if (WIFEXITED(status)) { // Check normal termination
                            if (status>>8 != 0){
                                printf("Le programme s'est terminé normalement avec erreur %d\n", WEXITSTATUS(status));
                            }else {
                               printf("Le programme s'est terminé normalement sans erreur %d\n", WEXITSTATUS(status)); 
                            }
                        } else if (WIFSIGNALED(status)) { // Check abnormal termination
                            printf("Le programme a été interrompu par le signal %d\n", WTERMSIG(status));
                        }
                    }
                }
            }
        }
    } else {
        perror("Erreur de fork");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}