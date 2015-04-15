#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

using namespace std;

void perror(const char *s);
pid_t fork(void);
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

int execvp(const char *file, char *const argv[]);

char *strtok(char *str, const char *delim);
char *strtok_r(char *str, const char *delim, char **saveptr);

int main(int argc, char *argv[]) {

//    char cmd[1024];
//    cout << cmd;
    int pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    else if (pid == 0) {
        if (execvp(argv[1], argv) == -1) {
            perror("argv[0]");
        }
    }




    return 0;
}
