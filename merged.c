/* * * * * * * * * *
 *
 *  main.c
 *  KeyVim
 
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

#ifndef HELPER_H
#define HELPER_H

void Threading(void *f1, void *f2);
void Wait(int ms);
void Clear(void);
#endif

#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#ifndef _WIN32

struct termios original;
void set_input_mode(void);
void reset_input_mode(void);

#endif

char Getchar(void);

#endif

//#include "outputhandler.h"

#define XSETUP_DELAY 100

/* * * * *
 * OUTPUT SECTION
 * * * * */

typedef struct _coor {
    int x;
    int y;
} coor;

coor Window;
char *Window_buffer;

coor Getwindowsize(void) {
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

coor Getcursorpos(void) {
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

    // 요청: ESC [6n
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

    // 응답 형식: ESC [ row ; col R
    if (buf[0] == '\033' && buf[1] == '[') {
        sscanf(&buf[2], "%d;%d", &pos.y, &pos.x);
    }

    return pos;
#endif
}


void Putchar(char a) {
    putc(a, stdout);
    return;
}

void Putchar_coor(char a, coor c) {
    Window_buffer[(Window.x+1) * c.y + c.x] = a;
    return;
}

void Putwindow(void) {
    coor p = get_cursor_pos();
    fwrite(Window_buffer, 1, strlen(Window_buffer), stdout);
    move_cursor_to(p);
    return;
}

void hide_cursor() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
#else
    printf("\033[?25l");
#endif
}

void show_cursor() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hOut, &cursorInfo);
#else
    printf("\033[?25h");
#endif
}

/* * * * *
 * ALGORITHM SECTION
 * * * * */

void xsetup(void);

void program_end(void) {
#ifndef _WIN32
    reset_input_mode();
#endif
    free(Window_buffer);
    return;
}

void setup(void) {
#ifndef _WIN32
    set_input_mode();
#endif
    atexit(program_end);
    setvbuf(stdout, NULL, _IONBF, 0);
    xsetup();
    Window = Getwindowsize();
    Window_buffer = (char*)malloc(sizeof(char) * (Window.x + 1) * Window.y + 1);
    // printf("%d %d", Window.x, Window.y);
    for (int i = 0; i < (Window.x + 1) * Window.y; ++i) {
        Window_buffer[i] = ' ';
    }
    for(int i=0;i<Window.y - 1;++i)
        Window_buffer[i * (Window.x + 1) + Window.x] = '\n';
    Window_buffer[(Window.y - 1) * (Window.x + 1) + Window.x] = '\0'; // null-terminator
    return;
}

void Defaultscreen(void) {
    Clear();
    for(int i = 0;i < Window.y;++i) {
        Window_buffer[(Window.x+1) * i] = '~';
    }
    return;
}

void game(void) {
    //Clear();
    Defaultscreen();
    coor k;
    k.x = 1;
    k.y = 1;
    Putchar_coor('k', k);
    Putwindow();
    while(1) {
        char k = Getchar();
        if(k == 'q') exit(0);
        Putchar(k);
    }
    return;
}

int options(void);


    options();
    return 0;
}


void xsetup(void) {
    Clear();
    printf("  _  __\n"
           " | |/ /\n"
           " | ' / \n"
           " |  <  \n"
           " | . \\ \n"
           " |_|\\_\\\n");
    Wait(XSETUP_DELAY);

    Clear();
    printf("  _  __\n"
           " | |/ /\n"
           " | ' / ___\n"
           " |  < / _ \\\n"
           " | . \\  __/\n"
           " |_|\\_\\___|\n");
    Wait(XSETUP_DELAY);

    Clear();
    printf("  _  __  \n"
           " | |/ /  \n"
           " | ' / ___ _   _ \n"
           " |  < / _ \\ | | |\n"
           " | . \\  __/ |_| | \n"
           " |_|\\_\\___|\\__, |\n"
           "            __/ |    \n"
           "           |___/    \n");
    Wait(XSETUP_DELAY);
    
    Clear();
    printf("  _  __       __      __\n"
           " | |/ /       \\ \\    / /\n"
           " | ' / ___ _   \\ \\  / / \n"
           " |  < / _ \\ | | \\ \\/ /  \n"
           " | . \\  __/ |_| |\\  /   \n"
           " |_|\\_\\___|\\__, | \\/    \n"
           "            __/ |       \n"
           "           |___/       \n");
    Wait(XSETUP_DELAY);

    Clear();
    printf("  _  __       __      ___ \n"
           " | |/ /       \\ \\    / (_)\n"
           " | ' / ___ _   \\ \\  / / _ \n"
           " |  < / _ \\ | | \\ \\/ / | |\n"
           " | . \\  __/ |_| |\\  /  | |\n"
           " |_|\\_\\___|\\__, | \\/   |_|\n"
           "            __/ |         \n"
           "           |___/        \n");
    Wait(XSETUP_DELAY);

    Clear();
    printf("  _  __       __      ___  \n"
           " | |/ /       \\ \\    / (_)\n"
           " | ' / ___ _   \\ \\  / / _ _ __ ___\n"
           " |  < / _ \\ | | \\ \\/ / | | '_ ` _ \\ \n"
           " | . \\  __/ |_| |\\  /  | | | | | | |\n"
           " |_|\\_\\___|\\__, | \\/   |_|_| |_| |_|\n"
           "            __/ |                  \n"
           "           |___/                   \n");
    Wait(XSETUP_DELAY);
}

int options(void) {
    printf("Welcome to KeyVim!\n[S] Start Game\n[O] Options\n[T] Tutorial\n[Q] Exit\nSelect an option: ");
    char input = Getchar();
    Putchar('\n');
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
}

#endif //PRG_DEBUG



#include <stdio.h>     // printf, fprintf
#include <stdlib.h>    // exit
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#endif

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

void Clear(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

/* TEST_START */
/*  */
/* void _f1() { */
/*     Wait(2000); */
/*     printf("f1 done\n"); */
/*     return; */
/* } */
/*  */
/* void _f2() { */
/*     Wait(3000); */
/*     printf("f2 done\n"); */
/*     return; */
/* } */
/*  */
/*     Threading((void*)_f1, (void*)_f2); */
/*     return 0; */
/* } */
/*  */
/* TEST_END */


#ifndef _WIN32
struct termios original;

void set_input_mode(void) {
    struct termios new;

    tcgetattr(STDIN_FILENO, &original); // backup current settings
    new = original;

    new.c_lflag &= ~(ICANON | ECHO | ISIG); // disable canonical mode + echo off + signal char disable
    new.c_cc[VMIN] = 1; // read at least 1 byte
    new.c_cc[VTIME] = 0; // no timeout

    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

void reset_input_mode(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &original); // restore
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


