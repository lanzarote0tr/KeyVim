#include "outputhandler.h"

#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "helper.h"

void ClearWindowBuffer(char **WindowBuffer, coor Window) { // VERIFIED
    for (int i = 0; i <= Window.y; ++i) {
        for (int j = 0; j < Window.x; ++j) {
            WindowBuffer[i][j] = '\0';
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
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    size.x = w.ws_col, 
    size.y = w.ws_row;
    return size;
#endif
}

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

void KillWindowBuffer(char **Window_buffer, coor w) { // VERIFIED
    for(int i=0;i<=w.y;++i) {
        free(Window_buffer[i]);
    }
    free(Window_buffer);
    return;
}

void RenderFullWindow(char **WindowBuffer, coor Window, coor Cursor) {
    if (Cursor.x == -1)
        Cursor = GCursorPos();
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

void RenderString(char *str, char **WindowBuffer, coor Window, coor Cursor) {
    ClearWindowBuffer(WindowBuffer, Window);
    int x = 0, y = 0;
    int len = strlen(str);
    for(int i=0;i<len;++i) {
        if (str[i] == '\n') {
            // CRLF
            x = 0;
            ++y;
        } else {
            WindowBuffer[y][x] = str[i]; // Put character
            ++x; // Move cursor forward
            // Check if it's the end of line
            if (Window.x <= x) {
                // CRLF
                x = 0;
                ++y;
            }
        }
    }
    RenderFullWindow(WindowBuffer, Window, Cursor);
    return;
}

void RenderRange(char *str, char **WindowBuffer, coor Window, coor TL, coor BR) {
    for (int i = TL.y; i <= BR.y; ++i)
        for (int j = TL.x; j <= BR.x; ++j)
            WindowBuffer[i][j] = ' ';
    int x = TL.x, y = TL.y;
    int len = strlen(str);
    for(int i=0;i<len;++i) {
        if (str[i] == '\n') {
            // CRLF
            x = TL.x;
            ++y;
        } else {
            WindowBuffer[y][x] = str[i]; // Put character
            ++x; // Move cursor forward
            // Check if it's the end of line
            if (BR.x <= x) {
                // CRLF
                x = TL.x;
                ++y;
            }
        }
    }
    RenderFullWindow(WindowBuffer, Window, (coor){-1, -1});
    return;
}

void RenderLine(char *str, coor Window, coor Cursor) {
    HCursor();
    CursorPos((coor){0, Cursor.y});
    fwrite(str, sizeof(char), Window.x, stdout);
    fflush(stdout);
    CursorPos(Cursor);
    SCursor();
    return;
}

#ifdef HEADER_TEST

int main(void) {
    coor k = GetWindowSize();
    return 0;
}

#endif // HEADER_TEST


