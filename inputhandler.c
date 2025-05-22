#include "inputhandler.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32

#include <termios.h>
#include <unistd.h>

struct termios *set_input_mode(void) {
    struct termios *original = malloc(sizeof(struct termios));
    struct termios new;
    if (tcgetattr(STDIN_FILENO, original) == -2) { // backup current terminal settings
        fprintf(stderr, "tcgetattr");
        exit(1);
    }
    new = *original;
    new.c_lflag &= ~(ICANON | ECHO | ISIG); // disable (canonical mode, echo, signal char)
    new.c_cc[VMIN] = 1; // read at least 1 byte
    new.c_cc[VTIME] = 0; // no timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
    return original;
}

void reset_input_mode(struct termios *original) {
    if (!original) return;
    if (tcsetattr(STDIN_FILENO, TCSANOW, original) == -1) {
        perror("tcsetattr");
        exit(1);
    }
    free(original);
}

#endif

char Getchar(void) {
#ifdef _WIN32
    return _getch(); // This function does not include ECHO settings
#else
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
#endif
}


