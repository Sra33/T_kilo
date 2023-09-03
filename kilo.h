#ifndef KILO_H_
#define KILO_H_
#include <asm-generic/errno-base.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ttydefaults.h>
#include <termios.h>
#include <unistd.h>

enum editorKey {
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};

struct editorConfig {
  int cx, cy;
  struct termios orig_termios;
  int screenrows;
  int screencols;
};

struct abuf {
  char *b;
  int len;
};

#define ABUF_INIT                                                              \
  { NULL, 0 }

#define CTRL_KEY(k) ((k)&0x1f)

void disableRawMode();
void enableRawMode();
void apAppend(struct abuf *, const char *, int);
void abfree(struct abuf *);
void initEditor();
void editorCursorMove(unsigned int);
void editorProcessKeyPress();
void editorDrawRows(sturct abuf *);
void editorRefreshScreen();

int getCursorPosition(int *, int *);
int getWindowSize(int *row, int *cols);
int editorReadKey();

#endif // KILO_H_
