/* * * * * * * * * *
 *
 *  outputhandler.c
 *  KeyVim
 *
 *  Created by Lanzarote(Ethan Won Cho) on 5/9/2025
 *  
 *  
 *  README!
 *  Do not compile this code as C++. If so, critical potential errors can occur. 
 *  Un-comment the type definition of coor when merging with main.c
 *
 * * * * * * * * * */
#include "outputhandler.h"
#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
// COMMENT THIS AT MERGE
#include "helper.h"
#include <unistd.h>

/* TODO: UN-COMMENT THIS AT MERGE
typedef struct _coor { // Coordinates
    unsigned int x;
    unsigned int y;
} coor;
*/

void ClearWindowBuffer(char **WindowBuffer, coor Window) { // VERIFIED
    for (int i = 0; i <= Window.y; ++i) {
        for (int j = 0; j < Window.x; ++j) {
            WindowBuffer[i][j] = ' ';
        }
        WindowBuffer[i][Window.x] = '\0';
    }
}

coor GetWindowSize(void) { // VERIFIED
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    coor size;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    size.x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    size.y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return size;
#else
    struct winsize w;
    coor size;
    int returnvalue = ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    size.x = w.ws_col, 
    size.y = w.ws_row;
    return size;
#endif
}

// CURSOR
coor GCursorPos(void) {
#ifdef _WIN32
    coor pos = {0, 0};
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    if (GetConsoleScreenBufferInfo(hStdout, &csbi)) {
        pos.y = csbi.dwCursorPosition.X;
        pos.x = csbi.dwCursorPosition.Y;
    }

    return pos;
#else
    coor pos = {0, 0};
    write(STDOUT_FILENO, "\033[6n", 4);
    fflush(stdout);
    char buf[32];
    int i = 0;
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';
    if (buf[0] == '\033' && buf[1] == '[') {
        sscanf(&buf[2], "%d%d", &pos.y, &pos.x);
    }
    return pos;
#endif
}

void CursorPos(coor cursor) {
#ifdef _WIN32
    COORD pos = {cursor.x, cursor.y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#else
    printf("\033[%d;%dH", cursor.y + 1, cursor.x + 1);
    fflush(stdout);
#endif
    return;
}

void HCursor(void) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
#else
    printf("\033[?25l");
    fflush(stdout);
#endif
    return;
}

void SCursor(void) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
#else
    printf("\033[?25h");
    fflush(stdout);
#endif
    return;
}

int MoveCursor(int k, coor *cursor, coor Scope) { // TODO: character detect and respond as is
    switch (k) {
    case 1: // Up Arrow
        if (cursor->y > 0) {
            --cursor->y;
            return 0;
        } else
            return 1;
        break;
    case 2: // Down Arrow
        if (cursor->y < Scope.y) {
            ++cursor->y;
            return 0;
        } else
            return 1;
        break;
    case 3: // Right Arrow
        if (cursor->x < Scope.x) {
            ++cursor->x;
            return 0;
        } else
            return 1;
        break;
    case 4: // Left Arrow
        if (cursor->x > 0) {
            --cursor->x;
            return 0;
        } else
            return 1;
        break;
    }
    return 2;
}


/********
9 * 3
x * y
012345678
901234567
890123456
********/

char **InitWindowBuffer(coor Window) { // VERIFIED
    char **Window_buffer = (char**)malloc((Window.y + 3) * sizeof(char*));
    for(int i=0;i<=Window.y;++i) {
        Window_buffer[i] = (char*)malloc((Window.x + 3) * sizeof(char));
    }
    return Window_buffer;
}

char *InitFileBuffer(void) {
    char *FileBuffer = (char*)malloc(10000 * sizeof(char*));
    for(int i=0;i<10000;++i) {
        FileBuffer[i] = '\0';
    }
    return FileBuffer;
}

void KillWindowBuffer(char **Window_buffer, coor w) { // VERIFIED
    for(int i=0;i<=w.y;++i) {
        free(Window_buffer[i]);
    }
    free(Window_buffer);
    return;
}

void RenderFullWindow(char **WindowBuffer, coor Window, coor Cursor) {
    ClearScreen();
    HCursor();
    CursorPos((coor){0, 0});
    for (int row = 0; row < Window.y; ++row) {
        /* Faster than printf for fixed-width writes and guarantees we
           overwrite any leftovers from the previous frame. */
        fwrite(WindowBuffer[row], 1, Window.x, stdout);

        /* Put a newline after every row except the last; this keeps the
           cursor parked at the bottom-right instead of dropping to the
           next line and causing scroll. */
        if (row != Window.y-1) fputc('\n', stdout); 
    }

    fflush(stdout);
    CursorPos(Cursor);
    SCursor();
    return;
}

void RenderLine(const char *line, coor Window, int y, coor Cursor) {
    HCursor();
    CursorPos((coor){0, y});
    fwrite(line, 1, strnlen(line, Window.x), stdout);
    if (strlen(line) < (size_t)Window.x)
        for (int i = strlen(line); i < Window.x; ++i)
            fputc(' ', stdout);

    fflush(stdout);
    CursorPos(Cursor);
    SCursor();
    return;
}

void RenderRange(const char *str, char **WindowBuffer, coor Window, coor TL, coor BR, coor Cursor) {
    for (int i = TL.y; i <= BR.y; ++i)
        for (int j = TL.x; j <= BR.x; ++j)
            WindowBuffer[i][j] = ' ';
    int x = TL.x;
    int y = TL.y;
    if (str != NULL) {
        int len = strlen(str);
        for(int i=0;i<len;++i) {
            if (str[i] == '\n') {
                // CRLF
                x = TL.x;
                ++y;
                if (BR.y <= y) {
                    perror("RenderRange: Out of Bound");
                    exit(1);
                }
            } else {
                WindowBuffer[y][x] = str[i]; // Put character
                ++x; // Move cursor forward
                // Check if it's the end of line
                if (BR.x <= x) {
                    // CRLF
                    x = TL.x;
                    ++y;
                    if (BR.y <= y) {
                        perror("RenderRange: Out of Bound");
                        exit(1);
                    }
                }
            }
        }
    }
    if (TL.y != BR.y) {
        RenderFullWindow(WindowBuffer, Window, Cursor);
    } else {
        RenderLine(WindowBuffer[TL.y], Window, TL.y, Cursor);
    }
    return;
}

void PutCharBuf(char c, char *FileBuffer, int FileCursor) {
    int len = strlen(FileBuffer);
    if (FileCursor < 0) FileCursor = 0;
    if (FileCursor > len) FileCursor = len;

    /* shift right, including the NUL terminator */
    for (int i = len; i >= FileCursor; --i)
        FileBuffer[i + 1] = FileBuffer[i];

    FileBuffer[FileCursor] = c;
}

void DelCharBuf(char *FileBuffer, int FileCursor) {
    int len = strlen(FileBuffer);
    for (int i=FileCursor-1;i<=len;++i) {
        FileBuffer[i] = FileBuffer[i+1];
    }
    FileBuffer[len] = '\0';
}

#ifdef HEADER_TEST

int main(void) {

    return 0;
}

#endif // HEADER_TEST


