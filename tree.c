/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

// Error handling
void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == 1)
        die("tcsetattr");
}

void enableRawMode() {
    // Read terminal attributes
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    // Disables flags and fixes control keys
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    // Apply terminal attributes
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

// Wait for a keypress and return it
char editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}

/*** output ***/

void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
}

/*** input ***/

// Handles keypresses
void editorProcessKeypress() {
    char c = editorReadKey();

    switch (c) {
    case CTRL_KEY('q'):
        exit(0);
        break;
    }
}

/*** init ***/

int main() {
    enableRawMode();

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
}
