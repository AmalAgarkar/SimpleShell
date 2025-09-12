#ifndef HEADER_H
#define HEADER_H

#include <bits/stdc++.h>
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <iostream>
#include <grp.h>
// #include <fstream>
#include <ctime>
#include <sstream>



#define HISTORY_LIMIT 20
using namespace std;

// Globals
extern vector<string> history;
extern int history_index;
extern pid_t fg_pid;
extern string fg_cmd;
extern const char* HISTORY_FILE;

//Main
void printGoodbye(); 

//  Core Shell Loop 
void shell_loop();
void execute_command(const string &cmd);
string get_prompt();

// Tokenizer
vector<string> tokenize(const string &input);
vector<string> split_commands(const string &input, char delimiter = ';');
vector<vector<string>> split_pipeline(const string &input);

//  Built-in Commands
void shell_cd(const vector<string> &args);
void shell_pwd();
void shell_echo(const vector<string> &args);
void shell_ls(const vector<string> &args);
void shell_history(const vector<string>& args);
void shell_pinfo(const vector<string> &args);
void shell_search(const vector<string> &args);
void shell_cat(const vector<string> &args);
void shell_sort(const vector<string> &args);
// void shell_wc(const vector<string> &args);
// void shell_head(const vector<string> &args);
// void shell_tail(const vector<string> &args);



// System Command Execution
void run_system_command(vector<string> &args);
void run_in_background(const vector<string> &args);

// I/O Redirection
int handle_redirection(vector<string> &args);

//  Pipeline
void execute_pipeline(vector<vector<string>> &commands);

// Job Control / Signals 
void init_signals();
void sigint_handler(int signo);
void sigtstp_handler(int signo);



// Get line
string _getline();
vector<string> get_local_files();
vector<string> get_path_commands();
vector<string> find_matches(const vector<string> &list, const string &prefix);
string common_prefix(const vector<string> &list);

// History
void save_history();
void load_history();
void add_history(const string &cmd);
void save_curr_history(const string &cmd);

#endif
