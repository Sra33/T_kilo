#include "kilo.h"
#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ttydefaults.h>
#include <unistd.h>

struct editorConfig {
  struct termios orig_termios;
  int screenrows;
  int screencols;
};

struct editorConfig E;

struct abuf {
  char *b;
  int len;
};

#define ABUF_INIT                                                              \
  { NULL, 0 }

/**
 * die - prints out error message to terminal
 * @s: const char , it is the message to sent to perror
 */
void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s);
  exit(1);
}

/*
 * abAppend - add string to buffer
 * @ab: a abuf struct
 * @s: const char *, string being added to the buffer
 * @len: lenght of string being added to buffer*/
void abAppend(struct abuf *ab, const char *s, int len) {
  char *new = realloc(ab->b, ab->len + len);

  if (new == NULL)
    return;
  memcpy(&new[ab->len], s, len);
  ab->b = new;
  ab->len += len;
}

/*
 * abFree - it is used as a destructor
 * @ab: a pointer to the abuf struct
 */
void abFree(struct abuf *ab) { free(ab->b); }

/*
 * getCursorPositon - this function reads the output of STDIN_FILENO which is
 * number of row and column Return: This function returns 0 on sucess and -1 on
 * failuer
 */
int getCursorPosition(int *row, int *column) {
  char buf[32];
  unsigned int i = 0;

  if (write(STDIN_FILENO, "\x1b[6n", 4) != 4)
    return -1;

  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1)
      break;
    if (buf[i] == 'R')
      break;
    i++;
  }
  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[')
    return -1;
  if (sscanf(&buf[2], "%d;%d", row, column) != 2)
    return -1;

  return 0;
}
/*
 * getWindowSize - this calls ioctl with the TIOCWINSZ request to get
 * terminal size
 * */
int getWindowSize(int *row, int *cols) {
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
      return -1;
    return getCursorPosition(row, cols);
  } else {
    *cols = ws.ws_col;
    *row = ws.ws_row;
    return 0;
  }
}

/*
 * initEditor - initialise all fields in the struct
 * */
void initEditor() {
  if (getWindowSize(&E.screenrows, &E.screencols) == -1)
    die("get Windowsize");
}

/**
 * disableRawMode - It sets the terminal to cooked mode by setting
 * echo attribute off
 *:w
 * */
void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) {
    die("tcsetattr");
  }
}

/**
 * enableRawMode - It sets the terminal to raw mode by setting
 * echo attribute off
 *:w
 * */
void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) {
    die("tcgetattr");
  }
  atexit(disableRawMode);

  struct termios raw = E.orig_termios;
  raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_lflag &= ~(OPOST);
  raw.c_lflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw.c_cc[VMIN] = 0; // sets the mini
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    die("tcsetattr");
  }
}
/**
 * editorReadkey - It reads the keypress from keyboard
 */
char editorReadKey() {
  int nread;
  char c;

  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      die("read");
  }
  return c;
}

void editorProcessKeyPress() {
  char c = editorReadKey();

  switch (c) {
  case CTRL_KEY('q'):
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    exit(0);
    break;
  }
}

/**
 * editorDrawrows - it handles drawing each row of the buffer of the text being
 * edited
 * */
void editorDrawRows(struct abuf *ab) {
  int y;
  for (y = 0; y < E.screenrows; y++) {
    abAppend(ab, "~", 1);

    abAppend(ab, "\x1b[K",
             4); // clears the part of the line right to the cursor

    if (y < E.screenrows - 1) {
      abAppend(ab, "\r\n", 2);
    }
  }
}

/**** OUTPUT ****/
/**
 * editorRefreshScreen - It clears the screen
 * the escape sequence to the terminal is /x1b[2J
 * x1b is the espcape character, it is usually followed by [, then an
 * argument where 2 means to clear the entire screen, 1 would clear the
 * screen up to where the cursor is, 0 would clear the screen from the
 * cursor to the end of the screen
 */
void editorRefreshScreen() {
  struct abuf ab = ABUF_INIT;

  abAppend(&ab, "\x1b[?25l", 6); // it is used for hiding cursor
  abAppend(&ab, "\x1b[H", 3);

  editorDrawRows(&ab);

  abAppend(&ab, "\x1b[H", 3);
  abAppend(&ab, "\x1b[?25h", 6); // it is used for showing cursor

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}

/**
 * Kilo is a text editor
 */
int main() {
  enableRawMode();
  initEditor();

  while (1) {
    editorRefreshScreen();
    editorProcessKeyPress();
  }
  return 0;
}
