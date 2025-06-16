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

/* TODO: UN-COMMENT THIS AT MERGE
typedef struct _coor { // Coordinates
    unsigned int x;
    unsigned int y;
} coor;
*/

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

void RenderRange(char *str, char **WindowBuffer, coor Window, coor TL, coor BR, coor Cursor) {
    for (int i = TL.y; i <= BR.y; ++i)
        for (int j = TL.x; j <= BR.x; ++j)
            WindowBuffer[i][j] = ' ';
    int x = TL.x, y = TL.y;
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
    RenderFullWindow(WindowBuffer, Window, Cursor);
    return;
}

void Putcharbuf(char c, char *FileBuffer, int FileCursor) {
    int len = strlen(FileBuffer);
    char next = '\0';
    for(int i=FileCursor;i<len;++i) {
        next = FileBuffer[i+1];
        FileBuffer[i+1] = FileBuffer[i];
    }
    FileBuffer[FileCursor] = c;
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
coor CICursor;
char ESCCount = 0;
char *FileBuffer;
int FileCursor = 0;
char samplecode1[] = "int convert_bit_range( int c, int from_bits, int to_bits )\n{\n\tint b = (1 << (from_bits - 1)) + c * ((1 << to_bits) - 1);\n\treturn (b + (b >> from_bits)) >> from_bits;\n}";

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
    CICursor = (coor){1, Window.y-2};
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
                RenderRange(NULL, WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, Cursor);
                CICursor = (coor){0, Window.y-2};
                return;
            }
        case 8:
        case 127: // Backspace
            CICursor.x -= 1;
            if (CICursor.x > 0) {
                WindowBuffer[CICursor.y][CICursor.x] = ' ';
                RenderFullWindow(WindowBuffer, Window, CICursor);
            } else { // Backspace until edge - Back to Normal
                RenderRange(NULL, WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, Cursor);
                CICursor = (coor){0, Window.y-1};
                return;
            }
            // TODO: 뒤쪽의 문자열 앞으로 옮기기
            break;
        case '\n': // Enter
            RenderRange(NULL, WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, CICursor);
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
            RenderRange(command, WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, CICursor);
            // RenderLine(WindowBuffer[CICursor.y], Window, CICursor);
        }
    }
}

void HandleInsertMode(void) {
    while (1) {
        char c = Getchar();
        if (c == 'q') exit(0);
        switch (c) {
        case 27: // ESC
            RenderRange("              ", WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, Cursor);
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
        default: // Render Real Time
            Putcharbuf(c, FileBuffer, FileCursor);
            ++FileCursor;
            ++Cursor.x;
            if (Cursor.x >= Window.x) {
                Cursor.x = 0;
                ++Cursor.y;
                // PrintError(""); TODO: OutofBound error
            }
            RenderRange(FileBuffer, WindowBuffer, Window, (coor){0, 0}, (coor){Window.x / 2, Window.y-3}, Cursor);
        }
    }
}

void HandleNormalMode(void) {
    while (1) {
        char c = Getchar();
        if (c == 'q') exit(0);
        switch (c) {
        case 'i':
            RenderRange("-- INSERT --", WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, Cursor);
            HandleInsertMode();
            break;
        case ':':
            WindowBuffer[Window.y-2][0] = ':';
            CICursor.x = 1;
            CICursor.y = Window.y-1;
            RenderFullWindow(WindowBuffer, Window, CICursor);
            HandleCommandMode();
            break;
        }
    }
}

void RenderTimer(void) {
    char str[10];
    for (int i=0;i<100;++i) {
        sprintf(str, "Time: %02d", i);
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
    Threading(HandleNormalMode, RenderTimer);
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

