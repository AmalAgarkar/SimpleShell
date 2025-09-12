#include "header.h"


string _getline() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    string line;
    int history_pos = history.size();

    char c;
    while (true) {
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n <= 0) break;

        if (c == 4) { // Ctrl+D 
            //cout << "Goodbye\n";
            printGoodbye();
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt); 
            exit(0);
        }
        else if (c == '\t') { // TAB for autocomplete
            vector<string> tokens = tokenize(line);
            string prefix;
            vector<string> candidates;

            if (tokens.empty() || (line.find_last_of(' ') == line.size()-1)) {
                
                prefix = "";
                candidates = get_local_files();
            } else {
                prefix = tokens.back();
                if (tokens.size() == 1) {
                    
                    candidates = get_path_commands();
                } else {
                    
                    candidates = get_local_files();
                }
            }

            vector<string> matches = find_matches(candidates, prefix);

            if (matches.empty()) {
                
            } 
            else if (matches.size() == 1) {
               
                string completion = matches[0].substr(prefix.size());
                line += completion;
                cout << completion << flush;
            } 
            else {
                
                string cp = common_prefix(matches);
                if (cp.size() > prefix.size()) {
               
                    string completion = cp.substr(prefix.size());
                    line += completion;
                    cout << completion << flush;
                } else {
                   
                    cout << "\n";
                    for (auto &m : matches) cout << m << "  ";
                    cout << "\n" << flush;
                    cout << line << flush; 
                }
            }
        }


        else if (c == '\n') {
            cout << "\n" << flush;
            break;
        } 
        else if (c == 127 || c == 8) { // Backspace
            if (!line.empty()) {
                line.pop_back();
                cout << "\b \b" << flush;
            }
        } 
else if (c == 27) { // Escape sequence
    char seq[2];
    if (read(STDIN_FILENO, &seq[0], 1) == 0) continue;
    if (read(STDIN_FILENO, &seq[1], 1) == 0) continue;

    if (seq[0] == '[') {
        if (seq[1] == 'A') { // UP arrow
            if (history_pos > 0) {
                history_pos--;

                // erase current input
                cout << "\r" << string(get_prompt().size() + line.size(), ' ') << "\r";
                cout << get_prompt();

                line = history[history_pos];
                cout << line << flush;
            }
        } 
        else if (seq[1] == 'B') { // DOWN arrow
            if (history_pos < (int)history.size() - 1) {
                history_pos++;

                cout << "\r" << string(get_prompt().size() + line.size(), ' ') << "\r";
                cout << get_prompt();

                line = history[history_pos];
                cout << line << flush;
            } 
            else if (history_pos == (int)history.size() - 1) {
                history_pos++;

                cout << "\r" << string(get_prompt().size() + line.size(), ' ') << "\r";
                cout << get_prompt();

                line.clear(); 
                cout << flush;
            }
        }
    }
}

        else { // Normal character
            line.push_back(c);
            cout << c << flush;   
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return line;
}

vector<string> get_path_commands() {
    vector<string> cmds;
    char *path_env = getenv("PATH");
    if (!path_env) return cmds;

    string path = path_env;
    stringstream ss(path);
    string dir;
    while (getline(ss, dir, ':')) {
        DIR *dp = opendir(dir.c_str());
        if (!dp) continue;
        struct dirent *entry;
        while ((entry = readdir(dp)) != NULL) {
      
            if (entry->d_type == DT_REG || entry->d_type == DT_LNK || entry->d_type == DT_UNKNOWN) {
                cmds.push_back(entry->d_name);
            }
        }
        closedir(dp);
    }
    return cmds;
}

// Get files in current directory
vector<string> get_local_files() {
    vector<string> files;
    DIR *dp = opendir(".");
    if (!dp) return files;
    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        files.push_back(entry->d_name);
    }
    closedir(dp);
    return files;
}

// Find matches from a list
vector<string> find_matches(const vector<string> &list, const string &prefix) {
    vector<string> matches;
    for (auto &s : list) {
        if (s.rfind(prefix, 0) == 0) 
            matches.push_back(s);
    }
    return matches;
}

// Longest common prefix
string common_prefix(const vector<string> &list) {
    if (list.empty()) return "";
    string prefix = list[0];
    for (size_t i = 1; i < list.size(); i++) {
        size_t j = 0;
        while (j < prefix.size() && j < list[i].size() && prefix[j] == list[i][j]) j++;
        prefix = prefix.substr(0, j);
    }
    return prefix;
}