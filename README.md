OS Assignment 2
Name- Amal Agarkar
Roll No- 2025202023

# Compile the  program
1. g++ command.cpp features.cpp get_line.cpp globals.cpp header.h loop.cpp main.cpp pipeline.cpp signals.cpp history.cpp -o shell

2. ./shell

# Code Workflow

1. main.cpp

    1. Prints a welcome banner.

    2. Initializes signals (init_signals()).

    3. Loads history from a file (load_history()).

    4. Starts the shell_loop().

    5. On exit, saves history (save_history() is commented out in main(), but it’s called from exit and Ctrl+D handling anyway).

2. loop.cpp – Core loop and command execution
Tokenization-

    1. tokenize() → splits input into tokens (ls -l > out.txt) becomes [ls, -l, >, out.txt].

    2. Handles:

        >whitespace separation

        >redirection symbols "<", ">", ">>"

        >pipelines "|"

        quoted strings "hello world"

Command Splitting-

    1. split_commands(input, ';') → allows multiple commands in one line.

    2. split_pipeline(input) → splits by | into a vector of commands.

execute_command()-

    1. Adds the full command line to history (add_history(cmd)).

    2. If it contains |, calls execute_pipeline().

        Otherwise:

            1. Tokenizes.
    
            2. Checks if last token is & → run in background.
    
            3. Handles redirections (<, >, >>) by replacing stdin/stdout.

    3. Dispatches built-ins:

        cd, pwd, echo, history, search, pinfo, ls, cat, sort

    4. Else runs external command with     run_system_command().

After execution, it restores original stdin/stdout.

shell_loop()-

1. Prints a prompt (get_prompt()).

2. Reads input with _getline() (supports history navigation).

3. If Ctrl+D → save history and exit.

4. Splits commands by ;, executes each with execute_command().

System Commands

    1. run_system_command() → Runs non-built-in commands in foreground:

    2. Handles <, >, >> redirection.

    3. Forks and execvp() in child.

    4. Parent waits, tracks foreground PID (fg_pid, fg_cmd) for signal handling.

    5. run_in_background() → Same, but doesn’t wait (just prints PID).

# Summary
This shell is essentially a mini POSIX-compliant shell with:

    1.Built-ins (cd, pwd, ls, cat, echo, history, search, pinfo, sort).

    2.External commands support via execvp.

    3.Pipelines (cmd1 | cmd2).

    4.Redirections (<, >, >>).

    5.Background jobs (&).

    6.History management with persistent save/load.

    7.Custom prompt.
