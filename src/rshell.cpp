#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string>

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
    int cap = 1000; // the capacity of the 2-D array
    char *cmd[cap]; // declare a 2-D 1000*1000 array
    char origStr[cap]; // store the command line
    char *save_1, *save_2; // save value using in strtok_r
    int i = 0;

    // pre-allocate cmd
    for (i = 0; i < cap; ++i) {
        cmd[i] = new char[cap];
    }

    // always in my rshell
    while (1) {
        cout << "$ ";

        memset(origStr, 0, cap); // initialize origStr to a empty string
        cin.getline(origStr, cap); // get the user input command line



        i = 0; // initialize i to 0 before the loop
        // split the whole line into command
        cmd[i] = strtok(origStr, " ");
        while (cmd[i] != NULL) {
            ++i;
            cmd[i] = strtok(NULL, " ");
        }


        int pid = fork(); // create a child process to call execvp()

        // fork’s return value for an error is -1
        if (pid == -1) {
            perror("fork");
            exit(1); //there was an error with fork so exit the program and go back and fix it
        }

        // this is in child process
        else if (pid == 0) {
            // run execvp in child process in order not to exit the whole program
            if (execvp(cmd[0], cmd) == -1) {
                perror(cmd[0]);
            }

            exit(1); // kill the child process when it's done
        }

        // this is in parent process
        else {
            int *status = 0;
            // wait for child process to finish executing
            if (wait(&status) == -1) {
                perror("wait()");
            }
        }


    }

    return 0;
}
