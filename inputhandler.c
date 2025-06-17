/* * * * * * * * * *
 *
 *  inputhandler.c
 *  KeyVim
 *
 *  Created by Lanzarote(Ethan Won Cho) on 5/9/2025
 *  
 *  
 *  README!
 *  Do not compile this code as C++. If so, critical potential errors can occur. 
 *
 * * * * * * * * * */
#include "inputhandler.h"

#include <stdio.h>
#include <stdlib.h>

#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_RIGHT 3
#define KEY_LEFT 4
#define KEY_ESC 27

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
    char c;
#ifdef _WIN32
    c = _getch(); // Without Echo Settigns
    if (ch == 0 || ch == 224) { // function/arrow prefix
        char ch = _getch();
        switch (ch) {
        case 72: return KEY_UP;    break;   // 0x48
        case 80: return KEY_DOWN;  break;   // 0x50
        case 75: return KEY_LEFT;  break;   // 0x4B
        case 77: return KEY_RIGHT; break;   // 0x4D
        default:  /* other keys */         break;
        }
    } else if (ch == 27) {
        key = KEY_ESC; // plain ESC
    }
#else
    read(STDIN_FILENO, &c, 1);
    if (c != 0x1B) return c;
    // ESC Detected. 10ms await for next key
    struct timeval tv = { .tv_sec = 0, .tv_usec = 10000 };
    fd_set rds;
    FD_ZERO(&rds);
    FD_SET(STDIN_FILENO, &rds);

    int k = select(STDIN_FILENO + 1, &rds, NULL, NULL, &tv);
    if (k <= 0)
        return KEY_ESC;

    /* Second byte waiting */
    read(STDIN_FILENO, &c, 1);
    if (c == '[')
        if (!(read(STDIN_FILENO, &c, 1))) return KEY_ESC;

    switch (c) {
        case 'A': return KEY_UP;
        case 'B': return KEY_DOWN;
        case 'C': return KEY_RIGHT;
        case 'D': return KEY_LEFT;
        default:  return KEY_ESC; // ignore F-keys, etc.
    }
#endif
}

