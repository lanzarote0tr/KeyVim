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


