#include "header.h"


int handle_redirection(vector<string> &args) {
    int in_fd = -1, out_fd = -1;
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "<" && i + 1 < args.size()) {
            in_fd = open(args[i + 1].c_str(), O_RDONLY);
            if (in_fd < 0) perror("open input");
            dup2(in_fd, STDIN_FILENO);
            args.erase(args.begin() + i, args.begin() + i + 2);
            i--;
        } else if (args[i] == ">" && i + 1 < args.size()) {
            out_fd = open(args[i + 1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd < 0) perror("open output");
            dup2(out_fd, STDOUT_FILENO);
            args.erase(args.begin() + i, args.begin() + i + 2);
            i--;
        } else if (args[i] == ">>" && i + 1 < args.size()) {
            out_fd = open(args[i + 1].c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (out_fd < 0) perror("open append");
            dup2(out_fd, STDOUT_FILENO);
            args.erase(args.begin() + i, args.begin() + i + 2);
            i--;
        }
    }
    return 0;
}

void execute_pipeline(vector<vector<string>> &commands) {
    int n = commands.size();
    vector<int> pipes(2 * (n - 1));

   
    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipes.data() + 2 * i) < 0) {
            perror("pipe");
            return;
        }
    }

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
           
            int in_fd = -1, out_fd = -1;
            bool redirect_in = false, redirect_out = false;

            vector<string> args;
            for (size_t j = 0; j < commands[i].size(); j++) {
                if (commands[i][j] == "<") {
                    in_fd = open(commands[i][j + 1].c_str(), O_RDONLY);
                    if (in_fd < 0) { perror("input redirection"); exit(1); }
                    redirect_in = true;
                    j++;
                }
                else if (commands[i][j] == ">") {
                    out_fd = open(commands[i][j + 1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (out_fd < 0) { perror("output redirection"); exit(1); }
                    redirect_out = true;
                    j++;
                }
                else if (commands[i][j] == ">>") {
                    out_fd = open(commands[i][j + 1].c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if (out_fd < 0) { perror("append redirection"); exit(1); }
                    redirect_out = true;
                    j++;
                }
                else {
                    args.push_back(commands[i][j]);
                }
            }

            if (i > 0) {
                dup2(pipes[2 * (i - 1)], STDIN_FILENO);
            }
            if (i < n - 1) {
                dup2(pipes[2 * i + 1], STDOUT_FILENO);
            }

            if (redirect_in) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (redirect_out) {
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
            }

           
            for (int k = 0; k < 2 * (n - 1); k++) close(pipes[k]);

            // DISPATCH CUSTOM COMMAND 
            if (args.empty()) exit(0);
            string cmd = args[0];
            if (cmd == "ls") shell_ls(args);
            else if (cmd == "cat") shell_cat(args);
            else if (cmd == "echo") shell_echo(args);
            else if (cmd == "sort") shell_sort(args);
            else if (cmd == "search") shell_search(args);
            // else if (cmd == "wc") shell_wc(args);
            // else if (cmd == "head") shell_head(args);
            // else if (cmd == "tail") shell_tail(args);
            else {
                vector<char*> argv;
                for (auto &s : args) {   
                    argv.push_back(const_cast<char*>(s.c_str()));
                }
                argv.push_back(NULL);

                execvp(argv[0], argv.data());
                cerr << "Unknown command: " << args[0] << "\n";
                exit(1);
            }

            exit(0);
        }
    }

    
    for (int k = 0; k < 2 * (n - 1); k++) close(pipes[k]);
    for (int i = 0; i < n; i++) wait(NULL);
}

