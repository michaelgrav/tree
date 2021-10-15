#include <termios.h>
#include <unistd.h>

void enableRawMode() {
    struct termios raw;

    // Read terminal attributes
    tcgetattr(STDIN_FILENO, &raw);

    // Disables each key typed to be printed to the terminal
    raw.c_lflag &= ~(ECHO);

    // Apply terminal attributes
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
    return 0;
}
