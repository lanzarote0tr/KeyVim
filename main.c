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
char FLAG = 0;
char *FileBuffer;
int FileCursor = 0;
int IsCommandMode = 0;
char SampleCode1[] = "asdf\nbrooo";
char SampleCode2[] = "int convert_bit_range( int c, int from_bits, int to_bits ) {\n    int b = (1 << (from_bits - 1)) + c * ((1 << to_bits) - 1);\n    return (b + (b >> from_bits)) >> from_bits;\n}";

void program_end(void) { // VERIFIED
#ifndef _WIN32
    reset_input_mode(original);
#endif
    // Return allocated memory
    KillWindowBuffer(WindowBuffer, Window);
    SCursor();
    return;
}

void DrawSampleCode(void) {
    Cursor.x = 0;
    Cursor.y = 0;
    RenderRange(SampleCode1, WindowBuffer, Window, (coor){Window.x/2, 0}, (coor){Window.x, Window.y-3}, Cursor);
    return;
}

int HandleCommand(char *cmd) {
    if (!strcmp(cmd, ":submit")) {
        if(!strcmp(FileBuffer, SampleCode1)) {
            return 1;
        } else return 2;
    }
    return 0;
}

// ESC, Function keys, backspace, tab, enter, arrow keys, keys with ^

int HandleCommandMode(void) {
    IsCommandMode = 1;
    CICursor = (coor){1, Window.y-2};
    char command[30];
    command[0] = ':';
    while (1) {
        char c = Getchar();
        if (c == 'q') exit(0);
        switch (c) {
        case 27: 
            RenderRange(NULL, WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, Cursor);
            CICursor = (coor){0, Window.y-2};
            IsCommandMode = 0;
            return 0;
            break;
        case KEY_UP: // Up Arrow
            perror("Up Arrow");
            exit(1);
            break;
        case KEY_DOWN: // Down Arrow
            perror("Down Arrow");
            exit(1);
            break;
        case KEY_RIGHT: // Right Arrow
            perror("Right Arrow");
            exit(1);
            break;
        case KEY_LEFT: // Left Arrow
            perror("Left Arrow");
            exit(1);
            break;
        case 8:
        case 127: // Backspace
            --CICursor.x;
            if (CICursor.x > 0) {
                WindowBuffer[CICursor.y][CICursor.x] = ' ';
                RenderFullWindow(WindowBuffer, Window, CICursor);
            } else { // Backspace until edge - Back to Normal
                RenderRange(NULL, WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, Cursor);
                CICursor = (coor){0, Window.y-2};
                IsCommandMode = 0;
                return 0;
            }
            // TODO: 뒤쪽의 문자열 앞으로 옮기기
            break;
        case '\n': // Enter
            RenderRange(NULL, WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, CICursor);
            command[CICursor.x] = '\0';
            int rst = HandleCommand(command);
            IsCommandMode = 0;
            switch (rst) {
            case 0: // No Problem
                CICursor = (coor){0, Window.y-2};
                return 0;
                break;
            case 1: // Success Submit!!
                return 1;
                break;
            case 2: // Fail Submit!!
                return 2;
                break;
            }
        default:
            // RTR (real time render)
            PutCharBuf(c, command, CICursor.x);
            ++CICursor.x;
            if (CICursor.x >= Window.x) {
                CICursor.x = 0;
                ++CICursor.y;
            }
            RenderRange(command, WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, CICursor);
            // RenderLine(WindowBuffer[CICursor.y], Window, CICursor);
        }
    }
}

void HandleInsertMode(void) {
    while (1) {
        char c = Getchar();
        int len;
        if (c == 'q') exit(0);
        switch (c) {
        case 27: // ESC
            RenderRange("              ", WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, Cursor);
            return;
        case 8:
        case 127: // Backspace
            len = strlen(FileBuffer);
            for (int i=FileCursor-1;i<=len;++i) {
                FileBuffer[i] = FileBuffer[i+1];
            }
            FileBuffer[len] = '\0';
            --FileCursor;
            if (MoveCursor(KEY_LEFT, &Cursor, (coor){Window.x / 2 - 1, Window.y - 3})) {
                MoveCursor(KEY_UP, &Cursor, (coor){Window.x / 2 - 1, Window.y - 3});
                while (!MoveCursor(KEY_RIGHT, &Cursor, (coor){Window.x / 2 - 1, Window.y - 3})) continue;
                
            }
            RenderRange(FileBuffer, WindowBuffer, Window, (coor){0, 0}, (coor){Window.x / 2-1, Window.y-3}, Cursor);
            // TODO: 뒤쪽의 문자열 앞으로 옮기기
            break;
        case '\n':
            PutCharBuf('\n', FileBuffer, FileCursor);
            ++FileCursor;
            while (!MoveCursor(KEY_LEFT, &Cursor, (coor){Window.x / 2 - 1, Window.y - 3}))
                continue;
            MoveCursor(KEY_DOWN, &Cursor, (coor){Window.x / 2 - 1, Window.y - 3});
            CursorPos(Cursor);
            break;
        default: // Render Real Time
            PutCharBuf(c, FileBuffer, FileCursor);
            ++FileCursor;
            ++Cursor.x;
            if (Cursor.x >= Window.x / 2 - 1) {
                perror("OutofBound");
                exit(1);
            }
            RenderRange(FileBuffer, WindowBuffer, Window, (coor){0, 0}, (coor){Window.x / 2-1, Window.y-3}, Cursor);
        }
    }
}

void HandleNormalMode(void) {
    while (1) {
        char c = Getchar();
        if (c == 'q') exit(0);
        switch (c) {
        case KEY_UP: // Up Arrow
        case 'k':
            MoveCursor(1, &Cursor, (coor){Window.x / 2 - 1, Window.y-3});
            CursorPos(Cursor);
            break;
        case KEY_DOWN: // Down Arrow
        case 'j':
            MoveCursor(2, &Cursor, (coor){Window.x / 2 - 1, Window.y-3});
            CursorPos(Cursor);
            break;
        case KEY_RIGHT: // Right Arrow
        case 'l':
            MoveCursor(3, &Cursor, (coor){Window.x / 2 - 1, Window.y-3});
            CursorPos(Cursor);
            break;
        case KEY_LEFT: // Left Arrow
        case 'h':
            MoveCursor(4, &Cursor, (coor){Window.x / 2 - 1, Window.y-3});
            CursorPos(Cursor);
            break;
        case 'i':
            RenderRange("-- INSERT --", WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, Cursor);
            HandleInsertMode();
            break;
        case 'a':
            ++Cursor.x;
            RenderRange("-- INSERT --", WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, Cursor);
            HandleInsertMode();
            break;
        case ':':
            WindowBuffer[Window.y-2][0] = ':';
            CICursor.x = 1;
            CICursor.y = Window.y-2;
            RenderFullWindow(WindowBuffer, Window, CICursor);
            int rst = HandleCommandMode();
            if (rst == 1) {
                ClearScreen();
                return;
            }
            break;
        }
    }
}

void RenderTimer(void) {
    char str[10];
    for (int i=0;i<10;++i) {
        sprintf(str, "Time: %02d", i);
        if (IsCommandMode)
            RenderRange(str, WindowBuffer, Window, (coor){0, Window.y-1}, (coor){20, Window.y-2}, CICursor);
        else
            RenderRange(str, WindowBuffer, Window, (coor){0, Window.y-1}, (coor){20, Window.y-2}, Cursor);
        Wait(1000);
    }
    return;
}

void game(void) {
    ClearScreen();
    ClearWindowBuffer(WindowBuffer, Window);
    FileBuffer = InitFileBuffer();
    Cursor = (coor){0, 0};
    DrawSampleCode();
    Threading(HandleNormalMode, RenderTimer);
    RenderRange("Congratulations!", WindowBuffer, Window, (coor){0, 0}, (coor){50, 0}, (coor){4, 4});
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
    showtitle();
    Window = GetWindowSize();
    // printf("%d %d", Window.x, Window.y);
    WindowBuffer = InitWindowBuffer(Window);
    ClearWindowBuffer(WindowBuffer, Window);
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

