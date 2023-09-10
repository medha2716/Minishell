#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>

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
int main()
{ // 21 in proc/pid/stat
    int delay;
    printf("Enter time delay: ");
    scanf("%d", &delay);
    char *inp = malloc(sizeof(char) * 100);
    char c;
    enableRawMode();
    int stdin_flags = fcntl(STDIN_FILENO, F_GETFL);

    fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK); // Set non-blocking input

    while (1)
    {
        printf("Prompt>\n");

        // Check if 'x' key is entered
        if (read(STDIN_FILENO, &c, 1) == 1 && c == 'x')
        {
            break; // Exit the loop when 'x' is entered
        }

        fflush(stdout); // Flush the output buffer to ensure prompt is displayed immediately

        sleep(delay); // Sleep for the specified delay
    }

    fcntl(STDIN_FILENO, F_SETFL, stdin_flags); // Restore blocking input
                                               // printf("\nInput Read: [%s]\n", inp);
    disableRawMode();
    return 0;
}