# Description

This a basic shell coded in C. 

This shell implements a set of user defined commands, including seek, peek, pastevents, warp, etc. (without background functionality).

Furthermore, this shell supports other system commands through the use of execvp.

Here's a brief description of this shell based on the provided specifications:

- **Prompt and Command Separation**: This shell displays a prompt for user input on each line. It supports both `;` and `&` as command separators, allowing users to execute multiple commands in sequence or in the background.

- **System Commands**: This shell can execute system commands like `emacs`, `gedit`, and others in both foreground and background processes.

- **Foreground and Background Processes**: This shell can run commands in the foreground, waiting for them to complete before regaining control, and in the background, allowing users to continue entering commands while the background process runs.

- **I/O Redirection**: This shell supports input and output redirection using symbols like `>`, `>>`, `<`, and pipes (`|`) to redirect input and output to/from files and between commands.

- **Process Management**: This shell maintains a list of running processes, including their command names, PIDs, and states (running or stopped). It allows users to send signals to processes and manage them with commands like `fg`, `bg`, and process interruption with `Ctrl-C`, `Ctrl-D`, and `Ctrl-Z`.

- **iMan Command**: The `iMan` command fetches man pages from the internet using sockets and outputs them to the terminal.


To run this shell, follow these steps:

1. Compile the shell using the `make` command in the terminal.

2. After successful compilation, you can run the shell by executing `./minishell`.




# Assumptions


The following assumptions have been made in the design and functionality of our custom shell:

### Part A
- As specified in the doubts document, peek - has not been implemented.
- The shell assumes a command (input line) length limit of 4096 bytes (5000 characters).
- For file paths, a maximum of 1024 bytes, conforming to the PATH_MAX limit, is allowed.
- When using the '-e' and '-l' flags with a single file, the shell displays on the terminal a maximum of 1e6 (1,000,000) bytes of text from the file.
- Exit Shell has not been implemented yet.
- When employing the '&' and ';' symbols concurrently within a command, a space needs to be inserted between them. For instance, the input `sleep 5 & ; sleep 3` is valid while `sleep 5 &; sleep 3` might not work as intended.
- For foreground processes that consist of multiple commands separated by ';', the shell displays the total time taken (when the elapsed time surpasses 2 seconds) for the entire process in the subsequent prompt.
- In the "pastevents" command, if an invalid position is provided, the shell doesn't store the invalid pastevents execute command at all.

### Part B
- Background processes cannot be created along with piping and redirection.
- For neonate, If anything other than 'x' is given as input , the pid continues to be printed. However, it goes to next prompt if signals (eg. Ctrl + C) are sent. (as per doubts document, signal handling was not required to be handled for neonate)
- "exit" is not being stored in the pastevents.
- For I/O redirection multiple input files (or) multiple output files cannot be used and will give error (or lead to undefined behaviour in some cases). 
- For activities, all processes that have not been stopped (process state: 'T') or killed are shown as 'Running' (according to specifications).

### Part C
- Printing either the `first 4096 bytes` of manpage after &lt;PRE&gt; tag or everything till &lt;/PRE&gt; tag (depending on which occurs first) for iMan. So, atleast NAME, SYNOPSIS and DESCRIPTION are always printed.


