#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#ifndef _WIN32

struct termios *set_input_mode(void);
void reset_input_mode(struct termios *original);

#endif

char Getchar(void);

#endif

