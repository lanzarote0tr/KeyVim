/* * * * * * * * * *
 *
 *  main.c
 *  KeyVim
 
 *  Created by Lanzarote(Ethan Won Cho) on 5/9/2025
 *  
 *  
 *  README!
 *  Do not compile this code as C++. If so, critical potential errors can occur. 
 *
 * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#endif

#include "helper.h"
#include "inputhandler.h"
#include "outputhandler.h"

#define XSETUP_DELAY 100

void xsetup(void);

struct termios *original;
coor Window;
char **WindowBuffer;
coor Cursor;

void program_end(void) {
#ifndef _WIN32
    reset_input_mode(original);
#endif
    KillWindowBuffer(WindowBuffer, Window);
    return;
}

void game(void) {
    Clear();
    ClearWindowBuffer(WindowBuffer, Window);
    Cursor.x = 0;
    Cursor.y = 0;
    char str[100];
    for(int i=0;i<100;++i) str[i] = '\0';
    int idx = 0;
    while (1) {
        char k = Getchar();
        str[idx] = k;
        ++idx;
        ++Cursor.x;
        RenderString(str, WindowBuffer, Window, Cursor);
        if(k == 'q') exit(0);
    }
    return;
}

int options(void);


int main(void) {
#ifndef _WIN32
    original = set_input_mode();
#endif
    atexit(program_end);
    setvbuf(stdout, NULL, _IONBF, 0);
    xsetup();
    Window = GetWindowSize();
    WindowBuffer = InitWindowBuffer(Window);
    for (int i = 0; i <= Window.y; ++i) {
        for (int j = 0; j < Window.x; ++j) {
            WindowBuffer[i][j] = '\0';
        }
        WindowBuffer[i][Window.x] = '\0';
    }
    RenderFullWindow(WindowBuffer, Window, (coor){-1, -1});
    options();
    return 0;
}


void xsetup(void) {
    Clear();
    printf("  _  __\n"
           " | |/ /\n"
           " | ' / \n"
           " |  <  \n"
           " | . \\ \n"
           " |_|\\_\\\n");
    Wait(XSETUP_DELAY);

    Clear();
    printf("  _  __\n"
           " | |/ /\n"
           " | ' / ___\n"
           " |  < / _ \\\n"
           " | . \\  __/\n"
           " |_|\\_\\___|\n");
    Wait(XSETUP_DELAY);

    Clear();
    printf("  _  __  \n"
           " | |/ /  \n"
           " | ' / ___ _   _ \n"
           " |  < / _ \\ | | |\n"
           " | . \\  __/ |_| | \n"
           " |_|\\_\\___|\\__, |\n"
           "            __/ |    \n"
           "           |___/    \n");
    Wait(XSETUP_DELAY);
    
    Clear();
    printf("  _  __       __      __\n"
           " | |/ /       \\ \\    / /\n"
           " | ' / ___ _   \\ \\  / / \n"
           " |  < / _ \\ | | \\ \\/ /  \n"
           " | . \\  __/ |_| |\\  /   \n"
           " |_|\\_\\___|\\__, | \\/    \n"
           "            __/ |       \n"
           "           |___/       \n");
    Wait(XSETUP_DELAY);

    Clear();
    printf("  _  __       __      ___ \n"
           " | |/ /       \\ \\    / (_)\n"
           " | ' / ___ _   \\ \\  / / _ \n"
           " |  < / _ \\ | | \\ \\/ / | |\n"
           " | . \\  __/ |_| |\\  /  | |\n"
           " |_|\\_\\___|\\__, | \\/   |_|\n"
           "            __/ |         \n"
           "           |___/        \n");
    Wait(XSETUP_DELAY);

    Clear();
    printf("  _  __       __      ___  \n"
           " | |/ /       \\ \\    / (_)\n"
           " | ' / ___ _   \\ \\  / / _ _ __ ___\n"
           " |  < / _ \\ | | \\ \\/ / | | '_ ` _ \\ \n"
           " | . \\  __/ |_| |\\  /  | | | | | | |\n"
           " |_|\\_\\___|\\__, | \\/   |_|_| |_| |_|\n"
           "            __/ |                  \n"
           "           |___/                   \n");
    Wait(XSETUP_DELAY);
}

int options(void) {
    RenderString("Welcome to KeyVim!\n[S] Start Game\n[O] Options\n[T] Tutorial\n[Q] Exit\nSelect an option: ", WindowBuffer, Window, (coor){-1, -1});
    char input = Getchar();
    putc('\n', stdout);
    switch(input) {
    case 'S':
    case 's':
        game();
        break;
    case 'O':
    case 'o':
        return 1;
        break;
    case 'T':
    case 't':
        return 2;
        break;
    case 'Q':
    case 'q':
        exit(0);
    default:
        printf("This option is not available. Please select again.\n");
        return options();
    }
    return -1;
}


