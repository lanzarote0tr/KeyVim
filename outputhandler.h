typedef struct _coor {
    unsigned int x;
    unsigned int y;
} coor;

void Clear(char **WindowBuffer, coor Window);
coor GetWindowSize(void);
coor GCursorPos(void);
void CursorPos(coor cursor);
void HCursor(void);
void SCursor(void);

char **InitWindowBuffer(coor w);
void KillWindowBuffer(char **Window_buffer, coor w);

void RenderFullWindow(char **WindowBuffer, coor Window, coor Cursor);
void RenderString(char *str, char **WindowBuffer, coor Window, coor Cursor);
void RenderRange(char *str, char **WindowBuffer, coor Window, coor TL, coor BR);
void RenderLine(char *str, coor Window, coor Cursor);

