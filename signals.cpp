#include "header.h"




extern pid_t fg_pid;
extern string fg_cmd;

// Handle Ctrl-C (SIGINT)
void sigint_handler(int sig) {
    if (fg_pid > 0) {
        kill(fg_pid, SIGINT);
        cout << endl;
        save_history();   
    }
}

// Handle Ctrl-Z (SIGTSTP)
void sigtstp_handler(int sig) {
    if (fg_pid > 0) {
        kill(fg_pid, SIGTSTP);
        cout << "\nStopped: " << fg_cmd << " [" << fg_pid << "]\n";
        
        fg_pid = -1;
        save_history();   
    }
}

// Initialize signals
void init_signals() {
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
}