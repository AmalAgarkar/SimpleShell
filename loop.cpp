#include "header.h"


//  TOKENIZATION 
vector<string> tokenize(const string &input) {
    vector<string> tokens;
    string token;

    for (size_t i = 0; i < input.size();) {
        if (isspace(input[i])) {
            i++;
            continue;
        }

        // Handle redirection symbols
        if (input[i] == '>') {
            if (i + 1 < input.size() && input[i + 1] == '>') {
                tokens.push_back(">>");
                i += 2;
            } else {
                tokens.push_back(">");
                i++;
            }
        }
        else if (input[i] == '<') {
            tokens.push_back("<");
            i++;
        }
        else if (input[i] == '|') {
            tokens.push_back("|");
            i++;
        }
        else if (input[i] == '"' || input[i] == '\'') {
            // Handle quoted strings
            char quote = input[i++];
            token.clear();
            while (i < input.size() && input[i] != quote) {
                token.push_back(input[i]);
                i++;
            }
            if (i < input.size() && input[i] == quote) {
                i++; 
            }
            tokens.push_back(token);
        }
        else {
            
            token.clear();
            while (i < input.size() && !isspace(input[i]) &&
                   input[i] != '<' && input[i] != '>' && input[i] != '|') {
                token.push_back(input[i]);
                i++;
            }
            tokens.push_back(token);
        }
    }

    return tokens;
}

vector<string> split_commands(const string &input, char delimiter) {
    vector<string> result;
    string temp;
    stringstream ss(input);
    while (getline(ss, temp, delimiter)) {
        if (!temp.empty()) result.push_back(temp);
    }
    return result;
}

vector<vector<string>> split_pipeline(const string &input) {
    vector<vector<string>> cmds;
    string temp;
    stringstream ss(input);
    while (getline(ss, temp, '|')) cmds.push_back(tokenize(temp));
    return cmds;
}



//  MAIN COMMAND EXECUTOR 
void execute_command(const string &cmd) {
    if (cmd.empty()) return;

    //add_history(cmd);
    auto cmds = split_pipeline(cmd);

    
    if (cmd.find('|') != string::npos) {
        execute_pipeline(cmds);
        return;
    }

    
    auto args = tokenize(cmd);
    if (args.empty()) return;

    
    if (args.back() == "&") { 
        args.pop_back(); 
        run_in_background(args); 
        return; 
    }

    // Handle redirection 
    int in_fd = -1, out_fd = -1;
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);

    vector<string> clean_args;
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "<") {
            if (i+1 < args.size()) {
                in_fd = open(args[i+1].c_str(), O_RDONLY);
                if (in_fd < 0) { perror("input redirection"); return; }
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
                i++;
            }
        }
        else if (args[i] == ">") {
            if (i+1 < args.size()) {
                out_fd = open(args[i+1].c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0644);
                if (out_fd < 0) { perror("output redirection"); return; }
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
                i++;
            }
        }
        else if (args[i] == ">>") {
            if (i+1 < args.size()) {
                out_fd = open(args[i+1].c_str(), O_WRONLY|O_CREAT|O_APPEND, 0644);
                if (out_fd < 0) { perror("append redirection"); return; }
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
                i++;
            }
        }
        else {
            clean_args.push_back(args[i]);
        }
    }

    if (clean_args.empty()) return;

    // Run built-ins or external
    if (clean_args[0] == "cd") shell_cd(clean_args);
    else if (clean_args[0] == "pwd") shell_pwd();
    else if (clean_args[0] == "echo") shell_echo(clean_args);
    else if (clean_args[0] == "history") shell_history(clean_args);
    else if (clean_args[0] == "search") shell_search(clean_args);
    else if (clean_args[0] == "pinfo") shell_pinfo(clean_args);
    else if (clean_args[0] == "ls") shell_ls(clean_args);
    else if (clean_args[0] == "cat") shell_cat(clean_args);
    else if (clean_args[0] == "sort") shell_sort(clean_args);  
    else if (clean_args[0] == "exit") {
        save_history();
        //cout << "Goodbye\n";
        printGoodbye();
        exit(EXIT_SUCCESS);
    }
    else {
        run_system_command(clean_args);
    }

    
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdin);
    close(saved_stdout);
}




//  MAIN LOOP
void shell_loop() {
    while (true) {
        // Show dynamic prompt
        string prompt = get_prompt();
        cout << prompt << flush;   // ensure prompt shows immediately

        // Read command line 
        string input = _getline();

        // if (cin.eof()) {  // user pressed Ctrl-D
        //     save_history();
        //     cout << "logout\n";
        //     exit(0);
        // }
        if (input.empty()) continue;

        
     
        add_history(input);
        
        // Split by ';' and run each command separately
        vector<string> cmds = split_commands(input, ';');
        for (auto &c : cmds) {
            execute_command(c);
        }

    }
}
