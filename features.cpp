#include "header.h"


string get_prompt() {
    char hostname[HOST_NAME_MAX];
    char cwd[PATH_MAX];

    // Username
    const char* username = getenv("USER");
    if (!username) {
        struct passwd* pw = getpwuid(getuid());
        username = pw ? pw->pw_name : "unknown";
    }

    // Hostname
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strcpy(hostname, "unknown");
    }

    // Current working directory
    if (!getcwd(cwd, sizeof(cwd))) {
        strcpy(cwd, "?");
    }

    // Replace $HOME with ~
    string dir = cwd;
    const char* home = getenv("HOME");
    if (home && dir.find(home) == 0) {
        dir.replace(0, strlen(home), "~");
    }

    return string(username) + "@" + string(hostname) + ":" + dir + "~> ";
}

// Run foreground 

void run_system_command(vector<string> &args) {
    int in_fd = -1, out_fd = -1;
    bool redirect_in = false, redirect_out = false, append_out = false;
    vector<char*> argv;

    // Parse arguments 
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i] == "<") {
            if (i + 1 < args.size()) {
                in_fd = open(args[i + 1].c_str(), O_RDONLY);
                if (in_fd < 0) { perror("input redirection"); return; }
                redirect_in = true;
                i++;
            }
        } 
        else if (args[i] == ">") {
            if (i + 1 < args.size()) {
                out_fd = open(args[i + 1].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (out_fd < 0) { perror("output redirection"); return; }
                redirect_out = true;
                i++;
            }
        } 
        else if (args[i] == ">>") {
            if (i + 1 < args.size()) {
                out_fd = open(args[i + 1].c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (out_fd < 0) { perror("append redirection"); return; }
                redirect_out = true;
                append_out = true;
                i++;
            }
        } 
        else {
            argv.push_back(const_cast<char*>(args[i].c_str()));
        }
    }
    argv.push_back(NULL);

    if (argv.empty()) return;

    //  Save stdin/stdout before redirection 
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);

    pid_t pid = fork();
    if (pid == 0) {
        //  Child process
        setpgid(0, 0);                  
        signal(SIGINT, SIG_DFL);   
        signal(SIGTSTP, SIG_DFL);       

        if (redirect_in)  { dup2(in_fd, STDIN_FILENO); close(in_fd); }
        if (redirect_out) { dup2(out_fd, STDOUT_FILENO); close(out_fd); }

        execvp(argv[0], argv.data());
        perror("execvp failed");
        exit(1);
    } 
    else if (pid > 0) {
        // Parent
        fg_pid = pid;
        fg_cmd = argv[0];

        int status;
        waitpid(pid, &status, WUNTRACED);

        // Reset foreground tracking if child exited/stopped
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            fg_pid = -1;
            fg_cmd = "";
        }

        //  Restore stdin/stdout 
        dup2(saved_stdin, STDIN_FILENO);
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdin);
        close(saved_stdout);
    } 
    else {
        perror("fork failed");
    }
}
//  Run background 
void run_in_background(const vector<string> &args) {
    pid_t pid = fork();
    if (pid == 0) {
        setpgid(0, 0);
        vector<char*> argv;
        for (auto &s : args) argv.push_back(const_cast<char*>(s.c_str()));
        argv.push_back(NULL);
        execvp(argv[0], argv.data());
        perror("execvp failed");
        exit(1);
    } else if (pid > 0) cout << "[bg] started pid: " << pid << "\n";
    else perror("fork failed");
}

//  Search

bool search_directory(const string &dir, const string &target) {
    DIR *dp = opendir(dir.c_str());
    if (!dp) return false;

    struct dirent *entry;
    while ((entry = readdir(dp))) {
        string name = entry->d_name;
        if (name == "." || name == "..") continue;

        string path = dir + "/" + name;

        //  exact filename match
        if (name == target) {
            closedir(dp);
            return true;
        }

        // recursive check in subdirectories
        if (entry->d_type == DT_DIR) {
            if (search_directory(path, target)) {
                closedir(dp);
                return true;
            }
        }
    }

    closedir(dp);
    return false;
}

void shell_search(const vector<string> &args) {
    if (args.size() < 2) { 
        cerr << "Usage: search <filename>\n"; 
        return; 
    }

    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    bool found = search_directory(cwd, args[1]);
    if (found) cout << "True\n";
    else cout << "False\n";
}



void shell_pinfo(const vector<string> &args) {
    pid_t pid;
    if (args.size() == 1) pid = getpid(); // default: current shell process
    else pid = stoi(args[1]);

    string path = "/proc/" + to_string(pid) + "/status";
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        cerr << "pinfo: process " << pid << " not found\n";
        return;
    }

    // Read file into buffer
    char buffer[8192];
    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    if (bytes <= 0) {
        cerr << "pinfo: unable to read process status\n";
        return;
    }
    buffer[bytes] = '\0';

    // Parse line by line
    char *line = strtok(buffer, "\n");
    while (line) {
        if (strncmp(line, "Name:", 5) == 0 ||
            strncmp(line, "State:", 6) == 0 ||
            strncmp(line, "Pid:", 4) == 0 ||
            strncmp(line, "PPid:", 5) == 0) {
            cout << line << "\n";
        }
        line = strtok(nullptr, "\n");
    }

    // Show executable path
    char exe_path[PATH_MAX];
    string exe_link = "/proc/" + to_string(pid) + "/exe";
    ssize_t len = readlink(exe_link.c_str(), exe_path, sizeof(exe_path) - 1);
    if (len != -1) {
        exe_path[len] = '\0';
        cout << "Executable: " << exe_path << "\n";
    }
}



