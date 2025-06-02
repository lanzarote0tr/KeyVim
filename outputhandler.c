#include "outputhandler.h"

#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "helper.h"

void ClearWindowBuffer(char **WindowBuffer, coor Window) {
    for (int i = 0; i <= Window.y; ++i) {
        for (int j = 0; j < Window.x; ++j) {
            WindowBuffer[i][j] = ' ';
        }
        WindowBuffer[i][Window.x] = '\0';
    }
}

coor GetWindowSize(void) {
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

coor GetCursorPos(void) {
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

void SetCursorPos(coor cursor) {
#ifdef _WIN32
    COORD pos = {cursor.x, cursor.y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#else
    printf("\033[%d;%dH", cursor.y, cursor.x);
    fflush(stdout);
#endif
    return;
}

void HideCursor(void) {
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

void ShowCursor(void) {
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

char **InitWindowBuffer(coor w) {
    char **Window_buffer = (char**)malloc((w.y + 3) * sizeof(char*));
    for(int i=0;i<=w.y;++i) {
        Window_buffer[i] = (char*)malloc((w.x + 3) * sizeof(char));
    }
    return Window_buffer;
}

void KillWindowBuffer(char **Window_buffer, coor w) {
    for(int i=0;i<=w.y;++i) {
        free(Window_buffer[i]);
    }
    free(Window_buffer);
    return;
}

void RenderFullWindow(char **WindowBuffer, coor Window, coor Cursor) {
    coor p = GetCursorPos();
    HideCursor();
    for(int i=0;i<=Window.y;++i) {
        SetCursorPos((coor){0, i});
        for(int j=0;j < Window.x;++j) {
            printf("%c", WindowBuffer[i][j]);
        }
    }
    fflush(stdout);
    if (Cursor.x == -1 && Cursor.y == -1) {
        SetCursorPos(p);
    } else {
        SetCursorPos(Cursor);
    }
    ShowCursor();
    return;
}

void RenderString(char *str, char **WindowBuffer, coor Window, coor Cursor) {
    //printf(str);
    Clear();
    Cursor.x = 0;
    Cursor.y = 0;
    int len = strlen(str);
    for(int i=0;i<len;++i) {
        if (str[i] == '\n') {
            // CRLF
            Cursor.x = 0;
            ++Cursor.y;
        } else {
            WindowBuffer[Cursor.y][Cursor.x] = str[i]; // Put character
            ++Cursor.x; // Move cursor forward
            // Check if it's the end of line
            if (Window.x <= Cursor.x) {
                // CRLF
                Cursor.x = 0;
                ++Cursor.y;
            }
        }
    }
    RenderFullWindow(WindowBuffer, Window, Cursor);
    return;
}

#ifdef HEADER_TEST

int main(void) {
    /*
    system("clear");
    for(int i=0;i<30;i++) {
        for(int j=0;j<30;j++) {
            printf("a");
        }printf("\n");
    }
    coor k = {3, 4};
    SetCursorPos(k);
    system("sleep 100");
    */
    coor k = GetWindowSize();
    printf("%d and %d\n", k.x, k.y);
    return 0;
}

#endif // HEADER_TEST


