#include "header.h"



// Get home directory path
string get_home_path() {
    const char* home = getenv("HOME");
    if (home) return string(home);
    return string(getpwuid(getuid())->pw_dir);
}

// Add a command to in-memory history
void add_history(const string &cmd) {
    if (cmd.empty()) return;
    history.push_back(cmd);

    // Append immediately to history file
    string histfile = get_home_path() + "/.my_shell_history";
    ofstream file(histfile, ios::app);
    if (file.is_open()) {
        file << cmd << '\n';
        file.close();
    }
}

// Load history from file (call at shell start)
void load_history() {
    string histfile = get_home_path() + "/.my_shell_history";
    ifstream file(histfile);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (!line.empty())
            history.push_back(line);
    }
    file.close();
}

// Save entire history (optional, e.g., on exit)
void save_history() {
    string histfile = get_home_path() + "/.my_shell_history";
    ofstream file(histfile);  // overwrite
    if (!file.is_open()) return;

    for (auto &cmd : history)
        file << cmd << '\n';

    file.close();
}