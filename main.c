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
int CUtime;
int ThreadFlag = 0;
int Level = 0;
float Record[10][3];
int RecordFlag = 0;
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
    for (int i=0;i<30;++i) command[i] = '\0';
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
            //TODO DelCharBuf()
            //TODO RenderRange()
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
            RenderRange(command, WindowBuffer, Window, (coor){0, Window.y - 1}, (coor){20, Window.y - 1}, CICursor);
            // RenderLine(WindowBuffer[CICursor.y], Window, CICursor);
        }
    }
}

void HandleInsertMode(void) {
    while (1) {
        char c = Getchar();
        switch (c) {
        case 27: // ESC
            RenderRange("              ", WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, Cursor);
            return;
        case 8:
        case 127: // Backspace
            DelCharBuf(FileBuffer, FileCursor);
            --FileCursor;
            if (MoveCursor(KEY_LEFT, &Cursor, (coor){Window.x / 2 - 1, Window.y - 3})) { // TODO : Unexpected Behavior
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
            RenderRange(FileBuffer, WindowBuffer, Window, (coor){0, 0}, (coor){Window.x / 2-1, Window.y-5}, Cursor);
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
            MoveCursor(1, &Cursor, (coor){Window.x / 2 - 1, Window.y-2});
            CursorPos(Cursor);
            break;
        case KEY_DOWN: // Down Arrow
        case 'j':
            MoveCursor(2, &Cursor, (coor){Window.x / 2 - 1, Window.y-2});
            CursorPos(Cursor);
            break;
        case KEY_RIGHT: // Right Arrow
        case 'l':
            MoveCursor(3, &Cursor, (coor){Window.x / 2 - 1, Window.y-2});
            CursorPos(Cursor);
            break;
        case KEY_LEFT: // Left Arrow
        case 'h':
            MoveCursor(4, &Cursor, (coor){Window.x / 2 - 1, Window.y-2});
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
                ClearWindowBuffer(WindowBuffer, Window);
                ClearScreen();
                ThreadFlag = 1;
                return;
            }
            break;
        }
    }
}

void RenderTimer(void) {
    CUtime = 0;
    char str[30];
    while (!ThreadFlag) {
        sprintf(str, "Time: %02d | %.2f char/s", CUtime, (float)FileCursor / CUtime);
        if (IsCommandMode)
            RenderRange(str, WindowBuffer, Window, (coor){0, Window.y-1}, (coor){30, Window.y-1}, CICursor);
        else RenderRange(str, WindowBuffer, Window, (coor){0, Window.y-1}, (coor){30, Window.y-1}, Cursor);
        Wait(1000);
        ++CUtime;
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
    char CongMessage[100];
    Record[RecordFlag][0] = (float)Level;
    Record[RecordFlag][1] = (float)CUtime;
    Record[RecordFlag][2] = (float)FileCursor / CUtime;
    sprintf(CongMessage, "Congratulations! You wrote in %d seconds!\nPress any key to continue playing...", CUtime);
    RenderRange(CongMessage, WindowBuffer, Window, (coor){0, 0}, (coor){50, 2}, (coor){4, 4});
    Getchar();
    ++RecordFlag;
    return;
}

void options(void) {
    RenderRange("[1] Level\n[2] Target Time", WindowBuffer, Window, (coor){0, 0}, (coor){20, 7}, (coor){17, 1});
    char c = Getchar();
    switch (c) {
    case '1':
        RenderRange("Select 1 to 5: ", WindowBuffer, Window, (coor){0, 0}, (coor){20, 7}, (coor){15, 0});
        char d = Getchar();
        Level = d - '0';
        RenderRange("Select 1 to 5: \nSuccessfully set!", WindowBuffer, Window, (coor){0, 0}, (coor){20, 7}, (coor){15, 0});
        Wait(SHOWTITLE_DELAY * 5);
        break;
    case '2':

        break;
    }
    return;
}

void leaderboard(void) {
    char str[140];
    sprintf(str, "LVL | RECORD | CPM\n%3.0f   %5.0f   %3.0f\n%3.0f   %5.0f   %3.0f\n", Record[0][0], Record[0][1], Record[0][2], Record[1][0], Record[1][1], Record[1][2]);
    RenderRange(str, WindowBuffer, Window, (coor){0, 0}, (coor){20, 7}, (coor){17, 1});
    char c = Getchar();
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
    while (1) {
        RenderRange("Welcome to KeyVim!\n[S] Start Game\n[O] Options\n[L] Leaderboard\n[Q] Exit\nSelect an option: ", WindowBuffer, Window, (coor){0, 0}, (coor){19, 7}, (coor){18, 5});
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
            options();
            break;
        case 'L':
        case 'l':
            leaderboard();
            break;
        case 'Q':
        case 'q':
            exit(0);
        default:
            printf("This option is not available. Please select again.\n");
            Wait(1000);
        }
    }
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

