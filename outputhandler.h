typedef struct _coor {
    unsigned int x;
    unsigned int y;
} coor;
coor GetWindowSize(void);
coor GetCursorPos(void);
void SetCursorPos(coor cursor);

char **InitWindowBuffer(coor w);
void KillWindowBuffer(char **Window_buffer, coor w);

void RenderWindow(char *Window_buffer);

