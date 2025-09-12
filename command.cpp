#include "header.h"

//  cd 
void shell_cd(const vector<string> &args) {
    static string prev_dir = ""; 
    const char* home = getenv("HOME");
    if (!home) home = getpwuid(getuid())->pw_dir;

    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    string current_dir = cwd;

    string target;

    if (args.size() < 2) {
        target = home; 
    } else {
        target = args[1];

        if (target == "~") {
            target = home;
        } 
        else if (target.rfind("~/", 0) == 0) {
            target = string(home) + target.substr(1);
        }
        else if (target == "-") {
            if (prev_dir.empty()) {
                cout << "cd: OLDPWD not set\n";
                return;
            }
            target = prev_dir;
            cout << target << "\n"; 
        }
    }

    if (chdir(target.c_str()) != 0) {
        perror("cd");
        return;
    }

    prev_dir = current_dir;
}


//pwd 
void shell_pwd() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd))) cout << cwd << "\n";
    else perror("pwd");
}

// echo 
void shell_echo(const vector<string> &args) {
    for (size_t i = 1; i < args.size(); i++) {
        cout << args[i];
        if (i + 1 < args.size()) cout << " ";
    }
    cout << "\n";
}
// ls
void shell_ls(const vector<string> &args) {
    string path = ".";
    bool show_all = false;
    bool long_format = false;

    const char* home = getenv("HOME");
    if (!home) home = getpwuid(getuid())->pw_dir;

    // Parse arguments
    for (size_t i = 1; i < args.size(); i++) {
        if (args[i] == "-a") {
            show_all = true;
        } else if (args[i] == "-l") {
            long_format = true;
        } else if (args[i][0] != '-') {
            path = args[i];
            
            if (path == "~") {
                path = home;
            } else if (path.rfind("~/", 0) == 0) { 
                path = string(home) + path.substr(1);
            }
        }
    }
    
    // Open directory
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        perror("ls");
        return;
    }
    
    struct dirent *entry;
    struct stat file_stat;
    
    while ((entry = readdir(dir)) != nullptr) {
        string filename = entry->d_name;
        
        if (!show_all && filename[0] == '.') {
            continue;
        }
        
        if (long_format) {
            string full_path = path + "/" + filename;
            if (stat(full_path.c_str(), &file_stat) == 0) {
               
                cout << ((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
                cout << ((file_stat.st_mode & S_IRUSR) ? "r" : "-");
                cout << ((file_stat.st_mode & S_IWUSR) ? "w" : "-");
                cout << ((file_stat.st_mode & S_IXUSR) ? "x" : "-");
                cout << ((file_stat.st_mode & S_IRGRP) ? "r" : "-");
                cout << ((file_stat.st_mode & S_IWGRP) ? "w" : "-");
                cout << ((file_stat.st_mode & S_IXGRP) ? "x" : "-");
                cout << ((file_stat.st_mode & S_IROTH) ? "r" : "-");
                cout << ((file_stat.st_mode & S_IWOTH) ? "w" : "-");
                cout << ((file_stat.st_mode & S_IXOTH) ? "x" : "-");
                
                
                cout << " " << file_stat.st_nlink;
                cout << " " << getpwuid(file_stat.st_uid)->pw_name;
                cout << " " << getgrgid(file_stat.st_gid)->gr_name;
                cout << " " << file_stat.st_size;
                
                
                char time_buf[80];
                strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", 
                        localtime(&file_stat.st_mtime));
                cout << " " << time_buf;
            }
        }
        
        cout << " " << filename << endl;
    }
    
    closedir(dir);
}

void shell_cat(const vector<string> &args) {
    const size_t BUF_SIZE = 4096;
    char buffer[BUF_SIZE];
    bool printed = false;  
    char lastChar = '\0';   

    if (args.size() > 1) {
        for (size_t i = 1; i < args.size(); i++) {
            int fd = open(args[i].c_str(), O_RDONLY);
            if (fd == -1) {
                cerr << "cat: " << args[i] << ": No such file or directory\n";
                continue;
            }

            ssize_t bytes;
            while ((bytes = read(fd, buffer, BUF_SIZE)) > 0) {
                write(STDOUT_FILENO, buffer, bytes);
                printed = true;
                lastChar = buffer[bytes - 1];
            }
            close(fd);
        }
    } 
    else {
        ssize_t bytes;
        while ((bytes = read(STDIN_FILENO, buffer, BUF_SIZE)) > 0) {
            write(STDOUT_FILENO, buffer, bytes);
            printed = true;
            lastChar = buffer[bytes - 1];
        }
    }

  
    if (printed && lastChar != '\n') {
        write(STDOUT_FILENO, "\n", 1);
    }
}

// history 
void shell_history(const vector<string>& args) {
    int n = 10; // default
    if (args.size() > 1) {
        try {
            n = stoi(args[1]);
        } catch (...) {
            cout << "history: invalid number\n";
            return;
        }
    }

    int start = max(0, (int)history.size() - n);
    for (int i = start; i < (int)history.size(); i++) {
        cout << history[i] << "\n";
    }
}
void shell_sort(const vector<string> &args) {
    vector<string> lines;
    string line;


    while (getline(cin, line)) {
        lines.push_back(line);
    }

    sort(lines.begin(), lines.end());

    for (const auto &l : lines) {
        cout << l << "\n";
    }
}


