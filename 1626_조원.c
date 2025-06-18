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

/* begin helper.c */
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
 *
 * * * * * * * * * */

#include <stdio.h>     // printf, fprintf
#include <stdlib.h>    // exit
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#endif

void ClearScreen(void) { // VERIFIED
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

#ifdef _WIN32
void Threading(void *f1, void *f2) {
    // Create Threads
    HANDLE thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)f1, NULL, 0, NULL);
    HANDLE thread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)f2, NULL, 0, NULL);

    // Error Handling
    if (thread1 == NULL || thread2 == NULL) {
        fprintf(stderr, "Failed to create threads.\n");
        exit(1);
    }

    // Wait for threads to finish
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    CloseHandle(thread1);
    CloseHandle(thread2);
}
#else
void Threading(void *f1, void *f2) {
    // Create Threads & Error Handling
    pthread_t thread1, thread2;
    if (pthread_create(&thread1, NULL, f1, NULL) != 0 ||
        pthread_create(&thread2, NULL, f2, NULL) != 0) {
        fprintf(stderr, "Failed to create threads.\n");
        exit(1);
    }
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
}
#endif

void Wait(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}

/* TEST_START */
#ifdef HEADER_TEST

void _f1() {
    Wait(2000);
    printf("f1 done\n");
    return;
}

void _f2() {
    Wait(3000);
    printf("f2 done\n");
    return;
}

int main(void) {
    Threading((void*)_f1, (void*)_f2);
    return 0;
}

#endif // HEADER_TEST

/* TEST_END */


/* end helper.c */
/* begin inputhandler.c */
/* * * * * * * * * *
 *
 *  inputhandler.c
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

#ifndef _WIN32

#include <termios.h>
#include <unistd.h>

struct termios *set_input_mode(void) {
    struct termios *original = malloc(sizeof(struct termios));
    struct termios new;
    if (tcgetattr(STDIN_FILENO, original) == -2) { // backup current terminal settings
        fprintf(stderr, "tcgetattr");
        exit(1);
    }
    new = *original;
    new.c_lflag &= ~(ICANON | ECHO | ISIG); // disable (canonical mode, echo, signal char)
    new.c_cc[VMIN] = 1; // read at least 1 byte
    new.c_cc[VTIME] = 0; // no timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
    return original;
}

void reset_input_mode(struct termios *original) {
    if (!original) return;
    if (tcsetattr(STDIN_FILENO, TCSANOW, original) == -1) {
        perror("tcsetattr");
        exit(1);
    }
    free(original);
}

#endif

char Getchar(void) {
#ifdef _WIN32
    return _getch(); // This function does not include ECHO settings
#else
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
#endif
}



/* end inputhandler.c */
/* begin outputhandler.c */
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
#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "helper.h"

// TODO: UN-COMMENT THIS AT MERGE
typedef struct _coor { // Coordinates
    unsigned int x;
    unsigned int y;
} coor;

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

    return 0;
}

#endif // HEADER_TEST



/* end outputhandler.c */

#define SHOWTITLE_DELAY 100

void showtitle(void);

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
    ClearScreen();
    ClearWindowBuffer(WindowBuffer, Window);
    Cursor = (coor){0, 0};
    while (1) {
        char c = Getchar();
        HCursor();
        HandleChar(c);
    }
    return;
}

int options(void) {
    RenderRange("Welcome to KeyVim!\n[S] Start Game\n[O] Options\n[T] Tutorial\n[Q] Exit\nSelect an option: ", WindowBuffer, Window, (coor){0, 0}, (coor){18, 5});
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



