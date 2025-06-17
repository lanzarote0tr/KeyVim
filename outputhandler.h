typedef struct _coor { // Coordinates
    unsigned int x;
    unsigned int y;
} coor;

void ClearWindowBuffer(char **WindowBuffer, coor Window);
coor GetWindowSize(void);

coor GCursorPos(void);
void CursorPos(coor cursor);
void HCursor(void);
void SCursor(void);
int MoveCursor(int k, coor *cursor, coor Scope);

char **InitWindowBuffer(coor w);
char *InitFileBuffer(void);
void KillWindowBuffer(char **Window_buffer, coor w);

void RenderFullWindow(char **WindowBuffer, coor Window, coor Cursor);
void RenderString(char *str, char **WindowBuffer, coor Window, coor Cursor);
void RenderRange(char *str, char **WindowBuffer, coor Window, coor TL, coor BR, coor Cursor);
void PutCharBuf(char c, char *FileBuffer, int FileCursor);

