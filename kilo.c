#include "kilo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  E.cx = 0;
  E.cy = 0;
  E.rowoff = 0;
  E.numrows = 0;
  E.row = NULL;

  if (getWindowSize(&E.screenrows, &E.screencols) == -1)
    die("get Windowsize");
}

/**
 * editorAppendRow - for opening and reading files from disk
 * @filename: a string that is the name of the file to be opened
 */
void editorAppendRow(char *s, size_t len) {
  E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));

  int at = E.numrows;

  E.row[at].size = len;
  E.row[at].chars = malloc(len + 1);
  memcpy(E.row[at].chars, s, len);
  E.row[at].chars[len] = '\0';
  E.numrows++;
}

/**
 * editorOpen - for opening and reading files from disk
 * @filename: a string that is the name of the file to be opened
 */
void editorOpen(char *filename) {

  FILE *fp = fopen(filename, "r");
  if (!fp)
    die("fopen");

  char *line = NULL;
  size_t linecap = 0;
  ssize_t lineLen;

  while ((lineLen = getline(&line, &linecap, fp)) != -1) {
    while (lineLen > 0 &&
           (line[lineLen - 1] == '\n' || line[lineLen - 1] == '\r')) {
      lineLen--;
      editorAppendRow(line, lineLen);
    }
  }
  free(line);
  fclose(fp);
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
int editorReadKey() {
  int nread;
  char c;

  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      die("read");
  }

  if (c == '\x1b') {
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return '\x1b';

    if (seq[0] == '[') {

      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1)
          return '\x1b';
        if (seq[2] == '~') {
          switch (seq[1]) {
          case '1':
            return HOME_KEY;
          case '2':
            return END_KEY;
          case '3':
            return DEL_KEY;
          case '5':
            return PAGE_UP;
          case '6':
            return PAGE_DOWN;
          case '7':
            return HOME_KEY;
          case '8':
            return END_KEY;
          }
        }
      } else {
        switch (seq[1]) {
        case 'A':
          return ARROW_UP;
        case 'B':
          return ARROW_DOWN;
        case 'C':
          return ARROW_RIGHT;
        case 'D':
          return ARROW_LEFT;
        case 'H':
          return HOME_KEY;
        case 'F':
          return END_KEY;
        }
      }
    } else if (seq[0] == '0') {
      switch (seq[1]) {
      case 'H':
        return HOME_KEY;
      case 'F':
        return END_KEY;
      }
    }

    return '\x1b';
  } else {
    return c;
  }
}

/*
 * editorCursorMove - this function moves the editor with the wasd commands
 * @key: char
 */

void editorCursorMove(unsigned int key) {
  erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

  switch (key) {
  case ARROW_LEFT:
    if (E.cx != 0) {
      E.cx--;
    } else if (E.cy > 0) {
      E.cy--;
      E.cx = E.row[E.cy].size;
    }
    break;
  case ARROW_RIGHT:
    if (row && E.cx < row->size) {
      E.cx++;
    } else if (row && E.cx == row->size) {
      E.cy++;
      E.cx = 0;
    }
    break;
  case ARROW_UP:
    if (E.cy != 0) {
      E.cy--;
    }
    break;
  case ARROW_DOWN:
    if (E.cy < E.numrows) {
      E.cy++;
    }
    break;
  }

  row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
  int rowlen = row ? row->size : 0;
  if (E.cx > rowlen) {
    E.cx = rowlen;
  }
}

void editorProcessKeyPress() {
  int c = editorReadKey();

  switch (c) {
  case HOME_KEY:
    E.cx = 0;
    break;

  case END_KEY:
    E.cy = 0;
    break;

  case CTRL_KEY('e'):
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    exit(0);
    break;

  case PAGE_UP:
  case PAGE_DOWN: {
    int times = E.screenrows;
    while (times--) {
      editorCursorMove(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    }
  }

  case ARROW_UP:
  case ARROW_DOWN:
  case ARROW_LEFT:
  case ARROW_RIGHT:
    editorCursorMove(c);
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
    int filerow = y + E.rowoff;
    if (filerow >= E.numrows) {
      if (E.numrows == 0 && y == E.screenrows / 3) {
        char welcome[80];
        int welcomelen = snprintf(welcome, sizeof(welcome),
                                  "Kilo editor -- version %s", KILO_VERSION);
        if (welcomelen > E.screencols)
          welcomelen = E.screencols;
        int padding = (E.screencols - welcomelen) / 2;
        if (padding) {
          abAppend(ab, "~", 1);
          padding--;
        }
        while (padding--)
          abAppend(ab, " ", 1);
        abAppend(ab, welcome, welcomelen);
      } else {
        abAppend(ab, "~", 1);
      }
    } else {
      int len = E.row[filerow].size - E.coloff;
      if (len < 0)
        len = 0;
      if (len > E.screencols)
        len = E.screencols;
      // possible bug
      abAppend(ab, &(E.row[filerow].chars[E.coloff]), len);
      // abAppend(ab, E.row[filerow].chars[E.coloff], len);
    }
    abAppend(ab, "\x1b[K", 3);
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
  editorScroll();
  struct abuf ab = ABUF_INIT;

  abAppend(&ab, "\x1b[?25l", 6); // it is used for hiding cursor
  abAppend(&ab, "\x1b[H", 3);

  editorDrawRows(&ab);

  char buf[32];
  // moving the cursor to a location specified
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1,
           (E.cx - E.coloff) + 1);
  abAppend(&ab, buf, strlen(buf));

  abAppend(&ab, "\x1b[?25h", 6); // it is used for showing cursor

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}

/**
 * editorScroll -  */
void editorScroll() {
  if (E.cy < E.rowoff) {
    E.rowoff = E.cy;
  }
  if (E.cy >= E.rowoff + E.screenrows) {
    E.rowoff = E.cy - E.screenrows + 1;
  }
  if (E.cx < E.coloff) {
    E.coloff = E.cx;
  }
  if (E.cx >= E.coloff + E.screencols) {
    E.coloff = E.cx - E.screencols + 1;
  }
}

/**
 * Kilo is a text editor
 */
int main(int argc, char *argv[]) {
  enableRawMode();
  initEditor();
  if (argc >= 2) {
    editorOpen(argv[1]);
  }

  while (1) {
    editorRefreshScreen();
    editorProcessKeyPress();
  }
  return 0;
}
