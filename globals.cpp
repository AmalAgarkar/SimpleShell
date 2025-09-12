#include "header.h"

// Shell history
vector<string> history;
int history_index = 0;
const char* HISTORY_FILE = ".myshell_history";

// Foreground process tracking
pid_t fg_pid = -1;
string fg_cmd = "";



