#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include "headers.h"

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

/**
 * Enable row mode for the terminal
 * The ECHO feature causes each key you type to be printed to the terminal, so you can see what you’re typing.
 * Terminal attributes can be read into a termios struct by tcgetattr().
 * After modifying them, you can then apply them to the terminal using tcsetattr().
 * The TCSAFLUSH argument specifies when to apply the change: in this case, it waits for all pending output to be written to the terminal, and also discards any input that hasn’t been read.
 * The c_lflag field is for “local flags”
 */
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

/**
 * stdout and stdin are buffered we disable buffering on that
 * After entering in raw mode we read characters one by one
 * Up arrow keys and down arrow keys are represented by 3 byte escape codes
 * starting with ascii number 27 i.e. ESC key
 * This way we interpret arrow keys
 * Tabs are usually handled by the term, but here we are simulating tabs for the sake of simplicity
 * Backspace move the cursor one control character to the left
 * @return
 */
int get_pid()
{

    // Open the loadavg file
    FILE *loadavg_file = fopen("/proc/loadavg", "r");
    if (!loadavg_file)
    {
        perror(MAG);
        perror("neonate: Error opening /proc/loadavg file");
        perror(COL_RESET);
        return 1;
    }

    // Read the required information from the loadavg file
    char *loadavg_buf = calloc(100001, sizeof(char));
    assert(loadavg_buf != NULL);
    fread(loadavg_buf, 1, 100000, loadavg_file);

    fclose(loadavg_file);

    // printf("%s\n", loadavg_buf);
    int no_of_tokens = 0;
    char **tokens = malloc(40 * sizeof(char *)); // array of tokens
    int tokens_array_size=40;
    char *delimiters = " ";
    char *token = strtok(loadavg_buf, delimiters);

    while (token != NULL)
    {
        tokens[no_of_tokens] = token;
        no_of_tokens++;

           if (no_of_tokens >= tokens_array_size)
        {
            tokens_array_size = (int)(tokens_array_size * 2);
            tokens = realloc(tokens, tokens_array_size * sizeof(char *));
        }

        token = strtok(NULL, delimiters);
    }

    tokens[no_of_tokens] = NULL;

    return atoi(tokens[4]);
}

int neonate(int delay)
{
    // The fifth field is the PID of the process that was most recently created on the system.
    // in proc/loadavg
    // # cat /proc/loadavg (input)
    // 0.75 0.35 0.25 1/25 1747 (output)
    // here 1747 is the pid
    
    if ((delay < 0))
    {
        printf(MAG);
        printf("neonate: Invalid time_arg\n");
        printf(COL_RESET);
        return 1;
    }
    int recent_pid = -1;

    char *inp = malloc(sizeof(char) * 100);
    char c;
    enableRawMode();

    // set non-blocking input
    int stdin_flags = fcntl(STDIN_FILENO, F_GETFL);
    fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK);

    struct timeval tv;
    fd_set readfds;

    while (1)
    {
        recent_pid = get_pid();
        printf("%d\n", recent_pid);

        // check if 'x' key is entered
        if (read(STDIN_FILENO, &c, 1) == 1 && c == 'x')
        {
            break; // exit the loop when 'x' is entered
        }

        fflush(stdout); // fflush buffer to ensure prompt is displayed immediately

        // set up a non-blocking wait for user input with a timeout
        tv.tv_sec = delay; // sleep for the specified delay
        tv.tv_usec = 0;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        // wait for input or timeout
        int ready = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
        if (ready < 0)
        {
            perror(MAG);
            perror("neonate: select");
            perror(COL_RESET);
            break;
        }
        else
        {
            // Data is available to be read from stdin, read and check for 'x'
            if (read(STDIN_FILENO, &c, 1) == 1 && c == 'x')
            {
                break;
            }
            fflush(stdout);
        }
    }

    fcntl(STDIN_FILENO, F_SETFL, stdin_flags); // Restore blocking input
    // printf("\nInput Read: [%s]\n", inp);
    disableRawMode();
  
    return 0;
}









