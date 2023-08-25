# Description

This is the first phase of a basic shell coded in C. 


# Assumptions


The following assumptions have been made in the design and functionality of our custom shell:

- As specified in the doubts document, peek - has not been implemented.
- The shell assumes a command (input line) length limit of 4096 bytes (5000 characters).
- For file paths, a maximum of 1024 bytes, conforming to the PATH_MAX limit, is allowed.
- When using the '-e' and '-l' flags with a single file, the shell displays on the terminal a maximum of 1e6 (1,000,000) bytes of text from the file.
- Exit Shell has not been implemented yet.
- When employing the '&' and ';' symbols concurrently within a command, a space needs to be inserted between them. For instance, the input `sleep 5 & ; sleep 3` is valid while `sleep 5 &; sleep 3` might not work as intended.
- For foreground processes that consist of multiple commands separated by ';', the shell displays the total time taken (when the elapsed time surpasses 2 seconds) for the entire process in the subsequent prompt.
- In the "pastevents" command, if an invalid position is provided, the shell doesn't store the invalid pastevents execute command at all.

