typedef struct _coor coor;

coor Window;
char *Window_buffer;

coor Getwindowsize(void);
coor Getcursorpos(void);
void Putchar(char a);
void Putchar_coor(char a, coor c);
void Putwindow(void);

void hide_cursor();
void show_cursor();

