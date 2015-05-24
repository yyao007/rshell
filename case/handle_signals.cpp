#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

struct sigaction *inter;
struct sigaction *quit;
struct sigaction *stop;
struct sigaction *origint;
struct sigaction *origstop;

int i = 0;
int j = 0;
int k = 0;

void interruputhdl(int signum, siginfo_t *info, void *ptr) {
    cout << "   c" << endl;
    ++i;
}

void quithdl(int signum, siginfo_t *info, void *ptr) {
    cout << "   \\" << endl;
    ++j;
}

void stophdl(int signum, siginfo_t *info, void *ptr) {
    cout << "   s" << endl;
    ++k;
    if (k >= 3) {
        cout << "\n^C: " << i << "\n^\\: " << j <<"\n^Z: " << k  << endl;
        exit(1);
    }
    raise(SIGSTOP);
}



int main() {
    inter = new struct sigaction;
    quit = new struct sigaction;
    stop = new struct sigaction;
    origstop = new struct sigaction;
    inter->sa_sigaction = interruputhdl;
    inter->sa_flags = SA_SIGINFO;
    quit->sa_sigaction = quithdl;
    quit->sa_flags = SA_SIGINFO;
    stop->sa_sigaction = stophdl;
    stop->sa_flags = SA_SIGINFO;
    sigaction(SIGINT, inter, NULL);
    sigaction(SIGQUIT, quit, NULL);
    sigaction(SIGTSTP, stop, NULL);

    while(1) {
        cout << "x" << endl;
        sleep(1);
    }

    delete inter;
    delete quit;
    delete stop;
    return 0;
}
