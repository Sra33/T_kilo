#include "kilo.h"
#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/ttydefaults.h>
#include <unistd.h>

struct editorConfig {
  struct termios orig_termios;
  int screenrows;
  int screencols;
};

struct editorConfig E;

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
 * getWindowSize - this calls ioctl with the TIOCWINSZ request to get
 * terminal size
 * */
int getWindowSize(int *row, int *cols) {
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    return -1;
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
void editorDrawRows() {
  int y;
  for (y = 0; y < E.screenrows; y++) {
    write(STDOUT_FILENO, "~/r/n", 3);
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
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  editorDrawRows();
  write(STDOUT_FILENO, "\x1b[H", 3);
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
