OS Assignment 2
Name- Amal Agarkar
Roll No- 2025202023

# Compile the  program
> g++ command.cpp features.cpp get_line.cpp globals.cpp header.h loop.cpp main.cpp pipeline.cpp signals.cpp history.cpp -o shell
> ./shell

# code workflow

1. main.cpp

>Prints a welcome banner.

>Initializes signals (init_signals()).

>Loads history from a file (load_history()).

>Starts the shell_loop().

>On exit, saves history (save_history() is commented out in main(), but it’s called from exit and Ctrl+D handling anyway).

2. loop.cpp – Core loop and command execution
Tokenization-

    >tokenize() → splits input into tokens (ls -l > out.txt) becomes [ls, -l, >, out.txt].

    >Handles:

        >whitespace separation

        >redirection symbols "<", ">", ">>"

        >pipelines "|"

        quoted strings "hello world"

Command Splitting-

    >split_commands(input, ';') → allows multiple commands in one line.

    >split_pipeline(input) → splits by | into a vector of commands.

execute_command()-

    >Adds the full command line to history (add_history(cmd)).

    >If it contains |, calls execute_pipeline().

    Otherwise:

        >Tokenizes.

        >Checks if last token is & → run in background.

        >Handles redirections (<, >, >>) by replacing stdin/stdout.

    >Dispatches built-ins:

        cd, pwd, echo, history, search, pinfo, ls, cat, sort

    >Else runs external command with     run_system_command().

After execution, it restores original stdin/stdout.

shell_loop()-

>Prints a prompt (get_prompt()).

>Reads input with _getline() (supports history navigation).

>If Ctrl+D → save history and exit.

>Splits commands by ;, executes each with execute_command().

System Commands

    >run_system_command() → Runs non-built-in commands in foreground:

    >Handles <, >, >> redirection.

    >Forks and execvp() in child.

    >Parent waits, tracks foreground PID (fg_pid, fg_cmd) for signal handling.

    >run_in_background() → Same, but doesn’t wait (just prints PID).

# summary
shell is essentially a mini POSIX-compliant shell with:

    1.Built-ins (cd, pwd, ls, cat, echo, history, search, pinfo, sort).

    2.External commands support via execvp.

    3.Pipelines (cmd1 | cmd2).

    4.Redirections (<, >, >>).

    5.Background jobs (&).

    6.History management with persistent save/load.

    7.Custom prompt.