/* * * * * * * * * *
 *
 *  main.c
 *  KeyVim
 *
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

#define SHOWTITLE_DELAY 100

void xsetup(void);

struct termios *original;
coor Window;
char **WindowBuffer;
coor Cursor;
coor CommandInputCursor;
char isCommandMode = 1;
char ESCCount = 0;

void program_end(void) { // VERIFIED
#ifndef _WIN32
    reset_input_mode(original);
#endif
    // Return allocated memory
    KillWindowBuffer(WindowBuffer, Window);
    SCursor();
    return;
}

void HandleChar(char c) {
    // ESC, Function keys, backspace, tab, enter, arrow keys, keys with ^
    if (c == 'q') exit(0);
    if (isCommandMode == 1) { // Normal
        switch (c) {
        case 'i':
            isCommandMode = 0;
            HCursor();
            strcpy(WindowBuffer[Window.y-1], "-- INSERT --\0");
            RenderLine(WindowBuffer[Window.y-1], Window, (coor){12, Window.y-1});
            CursorPos(Cursor);
            SCursor();
            break;
        case ':':
            isCommandMode = 2;
            WindowBuffer[Window.y-1][0] = ':';
            CommandInputCursor.x = 1;
            CommandInputCursor.y = Window.y-1;
            RenderLine(WindowBuffer[CommandInputCursor.y], Window, CommandInputCursor);
            break;
        }
    } else if (isCommandMode == 2) { // After pressing colon or slash etc.
        switch (c) {
        case 27:
            ++ESCCount;
            if (ESCCount == 2) {
                ESCCount = 0;
                isCommandMode = 1;
                HCursor();
                strcpy(WindowBuffer[Window.y-1], "            ");
                RenderLine(WindowBuffer[Window.y-1], Window, (coor){0, Window.y-1});
                CursorPos(Cursor);
            }
            break;
        case 8:
            printf("asdf");
            ESCCount = 0;
            //if (Cursor.x > 0) {
                WindowBuffer[Cursor.y][Cursor.x] = ' ';
                Cursor.x -= 1;
                RenderFullWindow(WindowBuffer, Window, Cursor);
                // 뒤쪽에 있는 문자열 앞으로 옮기기
            //}
            break;
        }
        // RTR (real time render)
        WindowBuffer[CommandInputCursor.y][CommandInputCursor.x] = c;
        ++CommandInputCursor.x;
        if (CommandInputCursor.x >= Window.x) {
            CommandInputCursor.x = 0;
            ++CommandInputCursor.y;
        }
        RenderLine(WindowBuffer[CommandInputCursor.y], Window, CommandInputCursor);
    } else { // INSERT MODE
        if (c == 27) { // ESC key
            isCommandMode = 1;
            HCursor();
            strcpy(WindowBuffer[Window.y-1], "             ");
            RenderLine(WindowBuffer[Window.y-1], Window, (coor){0, Window.y-1});
            CursorPos(Cursor);
        } else {
            // RTR
            WindowBuffer[Cursor.y][Cursor.x] = c;
            ++Cursor.x;
            if (Cursor.x >= Window.x) {
                Cursor.x = 0;
                ++Cursor.y;
            }
            RenderLine(WindowBuffer[Cursor.y], Window, Cursor);
        }
    }
}

void game(void) {
    Clear(WindowBuffer, Window);
    Cursor.x = 0;
    Cursor.y = 0;
    while (1) {
        char c = Getchar();
        HCursor();
        HandleChar(c);
    }
    return;
}

int options(void) {
    RenderString("Welcome to KeyVim!\n[S] Start Game\n[O] Options\n[T] Tutorial\n[Q] Exit\nSelect an option: ", WindowBuffer, Window, (coor){18, 5});
    char input = Getchar();
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

int main(void) {
#ifndef _WIN32
    original = set_input_mode();
    system("chcp 65001 > nul");
#endif
    atexit(program_end);
    setvbuf(stdout, NULL, _IONBF, 0);
    Window = GetWindowSize();
    WindowBuffer = InitWindowBuffer(Window);
    ClearWindowBuffer(WindowBuffer, Window);
    showtitle();
    ClearWindow();
    options();
    return 0;
}


void showtitle(void) {
    Clear(WindowBuffer, Window);
    printf("  _  __\n"
           " | |/ /\n"
           " | ' / \n"
           " |  <  \n"
           " | . \\ \n"
           " |_|\\_\\\n");
    Wait(SHOWTITLE_DELAY);

    Clear(WindowBuffer, Window);
    printf("  _  __\n"
           " | |/ /\n"
           " | ' / ___\n"
           " |  < / _ \\\n"
           " | . \\  __/\n"
           " |_|\\_\\___|\n");
    Wait(SHOWTITLE_DELAY);

    Clear(WindowBuffer, Window);
    printf("  _  __  \n"
           " | |/ /  \n"
           " | ' / ___ _   _ \n"
           " |  < / _ \\ | | |\n"
           " | . \\  __/ |_| | \n"
           " |_|\\_\\___|\\__, |\n"
           "            __/ |    \n"
           "           |___/    \n");
    Wait(SHOWTITLE_DELAY);
    
    Clear(WindowBuffer, Window);
    printf("  _  __       __      __\n"
           " | |/ /       \\ \\    / /\n"
           " | ' / ___ _   \\ \\  / / \n"
           " |  < / _ \\ | | \\ \\/ /  \n"
           " | . \\  __/ |_| |\\  /   \n"
           " |_|\\_\\___|\\__, | \\/    \n"
           "            __/ |       \n"
           "           |___/       \n");
    Wait(SHOWTITLE_DELAY);

    Clear(WindowBuffer, Window);
    printf("  _  __       __      ___ \n"
           " | |/ /       \\ \\    / (_)\n"
           " | ' / ___ _   \\ \\  / / _ \n"
           " |  < / _ \\ | | \\ \\/ / | |\n"
           " | . \\  __/ |_| |\\  /  | |\n"
           " |_|\\_\\___|\\__, | \\/   |_|\n"
           "            __/ |         \n"
           "           |___/        \n");
    Wait(SHOWTITLE_DELAY);

    Clear(WindowBuffer, Window);
    printf("  _  __       __      ___  \n"
           " | |/ /       \\ \\    / (_)\n"
           " | ' / ___ _   \\ \\  / / _ _ __ ___\n"
           " |  < / _ \\ | | \\ \\/ / | | '_ ` _ \\ \n"
           " | . \\  __/ |_| |\\  /  | | | | | | |\n"
           " |_|\\_\\___|\\__, | \\/   |_|_| |_| |_|\n"
           "            __/ |                  \n"
           "           |___/                   \n");
    Wait(SHOWTITLE_DELAY);
}



