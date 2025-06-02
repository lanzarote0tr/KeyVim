typedef struct _coor {
    unsigned int x;
    unsigned int y;
} coor;

void ClearWindowBuffer(char **WindowBuffer, coor Window);
coor GetWindowSize(void);
coor GetCursorPos(void);
void SetCursorPos(coor cursor);
void HideCursor(void);
void ShowCursor(void);

char **InitWindowBuffer(coor w);
void KillWindowBuffer(char **Window_buffer, coor w);

void RenderFullWindow(char **WindowBuffer, coor Window, coor Cursor);
void RenderString(char *str, char **WindowBuffer, coor Window, coor Cursor);

