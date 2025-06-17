#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_RIGHT 3
#define KEY_LEFT 4
#define KEY_ESC 27

#ifndef _WIN32

struct termios *set_input_mode(void);
void reset_input_mode(struct termios *original);

#endif

char Getchar(void);

#endif

