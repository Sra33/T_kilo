#ifndef KILO_H_
#define KILO_H_
#include <asm-generic/errno-base.h>
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ttydefaults.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define KILO_VERSION "1"
/*
 * erow - data type for storing row of text
 * @size: int , size of the data
 * @chars: pointer to char
 */
typedef struct erow {
  int size;
  char *chars;
} erow;

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
  int screenrows;
  int screencols;
  int numrows;
  int rowoff;
  int coloff;
  erow *row;
  struct termios orig_termios;
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
void editorScroll();
void apAppend(struct abuf *, const char *, int);
void abfree(struct abuf *);
void initEditor();
void editorOpen(char *);
void editorCursorMove(unsigned int);
void editorProcessKeyPress();
void editorDrawRows(struct abuf *);
void editorRefreshScreen();
void editorAppendRow(char *, size_t);

int getCursorPosition(int *, int *);
int getWindowSize(int *row, int *cols);
int editorReadKey();

#endif // KILO_H_
