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

void showtitle(void);

struct termios *original;
coor Window;
char **WindowBuffer;
coor Cursor;
coor CICursor;
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

void HandleCommand(char *cmd) {

    return;
}

// ESC, Function keys, backspace, tab, enter, arrow keys, keys with ^

void HandleCommandMode(void) {
    CICursor = (coor){1, Window.y-1};
    char command[30];
    int cmdlength = 1;
    while (1) {
        char c = Getchar();
        if (c == 'q') exit(0);
        switch (c) {
        case 27: 
            if (Getchar() == '[') {
                char c2 = Getchar();
                switch (c2) {
                case 'A': // Up Arrow - Command History
                    perror("Up Arrow");
                    exit(1);
                    break;
                case 'B': // Down Arrow - Command History
                    perror("Down Arrow");
                    exit(1);
                    break;
                case 'C': // Right Arrow - Cursor Move
                    perror("Right Arrow");
                    exit(1);
                    break;
                case 'D': // Left Arrow - Cursor Move
                    perror("Left Arrow");
                    exit(1);
                    break;
                }
            } else { // ESC - Back to Normal
                RenderRange(NULL, WindowBuffer, Window, (coor){0, Window.y - 1}, (coor){20, Window.y - 1}, Cursor);
                CICursor = (coor){0, Window.y-1};
                return;
            }
        case 8:
        case 127: // Backspace
            CICursor.x -= 1;
            if (CICursor.x > 0) {
                WindowBuffer[CICursor.y][CICursor.x] = ' ';
                RenderFullWindow(WindowBuffer, Window, CICursor);
            } else { // Backspace until edge - Back to Normal
                RenderRange(NULL, WindowBuffer, Window, (coor){0, Window.y - 1}, (coor){20, Window.y - 1}, Cursor);
                CICursor = (coor){0, Window.y-1};
                return;
            }
            // TODO: 뒤쪽의 문자열 앞으로 옮기기
            break;
        case '\n': // Enter
            RenderRange(NULL, WindowBuffer, Window, (coor){0, Window.y - 1}, (coor){20, Window.y - 1}, Cursor);
            command[CICursor.x] = '\0';
            HandleCommand(command);
            CICursor = (coor){0, Window.y-1};
            return;
        default:
            // RTR (real time render)
            WindowBuffer[CICursor.y][CICursor.x] = c;
            command[CICursor.x-1] = c;
            ++CICursor.x;
            if (CICursor.x >= Window.x) {
                CICursor.x = 0;
                ++CICursor.y;
            }
            RenderRange(command, WindowBuffer, Window, (coor){0, Window.y - 1}, (coor){20, Window.y - 1}, Cursor);
            // RenderLine(WindowBuffer[CICursor.y], Window, CICursor);
        }
    }
}

void HandleInsertMode(void) {
    while (1) {
        char c = Getchar();
        if (c == 'q') exit(0);
        switch (c) { // ESC key
        case 27: // ESC
            RenderRange("              ", WindowBuffer, Window, (coor){0, Window.y - 1}, (coor){20, Window.y - 1}, Cursor);
            return;
        case 8:
        case 127: // Backspace
            WindowBuffer[Cursor.y][Cursor.x-1] = ' ';
            if (Cursor.x > 0)
                Cursor.x -= 1;
            else
                Cursor.x = Window.x - 1;
                RenderFullWindow(WindowBuffer, Window, Cursor);
                // TODO: 뒤쪽의 문자열 앞으로 옮기기
            break;
        default:
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

void HandleNormalMode(void) {
    while (1) {
        char c = Getchar();
        if (c == 'q') exit(0);
        switch (c) {
        case 'i':
            RenderRange("-- INSERT --", WindowBuffer, Window, (coor){0, Window.y - 1}, (coor){20, Window.y - 1}, Cursor);
            HandleInsertMode();
            break;
        case ':':
            WindowBuffer[Window.y-1][0] = ':';
            CICursor.x = 1;
            CICursor.y = Window.y-1;
            RenderFullWindow(WindowBuffer, Window, CICursor);
            HandleCommandMode();
            break;
        }
    }
}

void game(void) {
    ClearScreen();
    ClearWindowBuffer(WindowBuffer, Window);
    Cursor = (coor){0, 0};
    HandleNormalMode();
    return;
}

int options(void) {
    RenderRange("Welcome to KeyVim!\n[S] Start Game\n[O] Options\n[T] Tutorial\n[Q] Exit\nSelect an option: ", WindowBuffer, Window, (coor){0, 0}, (coor){19, 7}, (coor){18, 5});
    char input = Getchar();
    printf("%c", input);
    Wait(SHOWTITLE_DELAY);
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
    ClearScreen();
    options();
    return 0;
}


void showtitle(void) {
    ClearScreen();
    printf("  _  __\n"
           " | |/ /\n"
           " | ' / \n"
           " |  <  \n"
           " | . \\ \n"
           " |_|\\_\\\n");
    Wait(SHOWTITLE_DELAY);

    ClearScreen();
    printf("  _  __\n"
           " | |/ /\n"
           " | ' / ___\n"
           " |  < / _ \\\n"
           " | . \\  __/\n"
           " |_|\\_\\___|\n");
    Wait(SHOWTITLE_DELAY);

    ClearScreen();
    printf("  _  __  \n"
           " | |/ /  \n"
           " | ' / ___ _   _ \n"
           " |  < / _ \\ | | |\n"
           " | . \\  __/ |_| | \n"
           " |_|\\_\\___|\\__, |\n"
           "            __/ |    \n"
           "           |___/    \n");
    Wait(SHOWTITLE_DELAY);
    
    ClearScreen();
    printf("  _  __       __      __\n"
           " | |/ /       \\ \\    / /\n"
           " | ' / ___ _   \\ \\  / / \n"
           " |  < / _ \\ | | \\ \\/ /  \n"
           " | . \\  __/ |_| |\\  /   \n"
           " |_|\\_\\___|\\__, | \\/    \n"
           "            __/ |       \n"
           "           |___/       \n");
    Wait(SHOWTITLE_DELAY);

    ClearScreen();
    printf("  _  __       __      ___ \n"
           " | |/ /       \\ \\    / (_)\n"
           " | ' / ___ _   \\ \\  / / _ \n"
           " |  < / _ \\ | | \\ \\/ / | |\n"
           " | . \\  __/ |_| |\\  /  | |\n"
           " |_|\\_\\___|\\__, | \\/   |_|\n"
           "            __/ |         \n"
           "           |___/        \n");
    Wait(SHOWTITLE_DELAY);

    ClearScreen();
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



