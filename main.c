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

void program_end(void) {
#ifndef _WIN32
    reset_input_mode(original);
#endif
    free(Window_buffer);
    return;
}

void Defaultscreen(void) {
    Clear();
    for(int i = 0;i < Window.y;++i) {
        Window_buffer[(Window.x+1) * i] = '~';
    }
    return;
}

void game(void) {
    //Clear();
    Defaultscreen();
    coor k;
    k.x = 1;
    k.y = 1;
    Putchar_coor('k', k);
    Putwindow();
    while(1) {
        char k = Getchar();
        if(k == 'q') exit(0);
        Putchar(k);
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
    Window = Getwindowsize();
    Window_buffer = (char*)malloc(sizeof(char) * (Window.x + 1) * Window.y + 1);
    // printf("%d %d", Window.x, Window.y);
    for (int i = 0; i < (Window.x + 1) * Window.y; ++i) {
        Window_buffer[i] = ' ';
    }
    for(int i=0;i<Window.y - 1;++i)
        Window_buffer[i * (Window.x + 1) + Window.x] = '\n';
    Window_buffer[(Window.y - 1) * (Window.x + 1) + Window.x] = '\0'; // null-terminator

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
    printf("Welcome to KeyVim!\n[S] Start Game\n[O] Options\n[T] Tutorial\n[Q] Exit\nSelect an option: ");
    char input = Getchar();
    Putchar('\n');
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
}


