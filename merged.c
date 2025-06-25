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

#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_RIGHT 3
#define KEY_LEFT 4
#define KEY_ESC 27

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
    char c;
#ifdef _WIN32
    c = _getch(); // Without Echo Settigns
    if (c == 0 || c == 224) { // function/arrow prefix
        char c = _getch();
        switch (c) {
        case 72: return KEY_UP;    break;   // 0x48
        case 80: return KEY_DOWN;  break;   // 0x50
        case 75: return KEY_LEFT;  break;   // 0x4B
        case 77: return KEY_RIGHT; break;   // 0x4D
        default:  /* other keys */         break;
        }
    } else if (c == 27) {
        return KEY_ESC; // plain ESC
    } else if (c == '\r') {
        return '\n';
    }
#else
    read(STDIN_FILENO, &c, 1);
    if (c != 0x1B) return c;
    // ESC Detected. 10ms await for next key
    struct timeval tv = { .tv_sec = 0, .tv_usec = 10000 };
    fd_set rds;
    FD_ZERO(&rds);
    FD_SET(STDIN_FILENO, &rds);

    int k = select(STDIN_FILENO + 1, &rds, NULL, NULL, &tv);
    if (k <= 0)
        return KEY_ESC;

    /* Second byte waiting */
    read(STDIN_FILENO, &c, 1);
    if (c == '[')
        if (!(read(STDIN_FILENO, &c, 1))) return KEY_ESC;

    switch (c) {
        case 'A': return KEY_UP;
        case 'B': return KEY_DOWN;
        case 'C': return KEY_RIGHT;
        case 'D': return KEY_LEFT;
        default:  return KEY_ESC; // ignore F-keys, etc.
    }
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



/* end outputhandler.c */

#define SHOWTITLE_DELAY 100

void showtitle(void);

struct termios *original;
coor Window;
char **WindowBuffer;
coor Cursor;
coor CICursor;
char *FileBuffer;
int FileCursor = 0;
int IsCommandMode = 0;
int ThreadFlag = 0;

int CUtime;
int Level = 0;
int GameCount = 0;
typedef struct {
    int level;
    int time;
    float cps;
} Record;
Record rec[10];
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
            RenderRange(command, WindowBuffer, Window, (coor){0, Window.y - 2}, (coor){20, Window.y - 2}, CICursor);
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
    rec[0].level = Level;
    rec[0].time = CUtime;
    rec[0].cps = (float)FileCursor / CUtime;
    char CongMessage[100];
    sprintf(CongMessage, "Congratulations! You wrote in %d seconds!\nPress any key to continue playing...", CUtime);
    RenderRange(CongMessage, WindowBuffer, Window, (coor){0, 0}, (coor){50, 2}, (coor){40, 2});
    CUtime = 0;
    ThreadFlag = 0;
    Getchar();
    ++GameCount;
    ClearScreen();
    ClearWindowBuffer(WindowBuffer, Window);
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
    sprintf(str, "LVL | RECORD | CPS");
    RenderRange(str, WindowBuffer, Window, (coor){0, 0}, (coor){20, 0}, (coor){0, 0});
    for (int i=0;i<=GameCount;++i) {
        char str[140];
        sprintf(str, "%03d   %03.9d   %3.0f", rec[i].level, rec[i].time, rec[i].cps);
        RenderRange(str, WindowBuffer, Window, (coor){0, i + 1}, (coor){20, i + 1}, (coor) {0,0});
    }
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

