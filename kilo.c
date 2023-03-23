#include "kilo.h"
#include <stdio.h>
#include <stdlib.h>

struct termios orig_termios;
/**
 * die - prints out error message to terminal
 * @s: const char , it is the message to sent to perror
 */
void die(const char *s) {
  perror(s);
  exit(1);
}

/**
 * disableRawMode - It sets the terminal to raw mode by setting
 * echo attribute off
 *:w
 * */
void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
    die("tcsetattr");
  }
}

/**
 * enableRawMode - It sets the terminal to raw mode by setting
 * echo attribute off
 *:w
 * */
void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
    die("tcgetattr");
  }
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_lflag &= ~(OPOST);
  raw.c_lflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    die("tcsetattr");
  }
}
/**
 * Kilo is a text editor
 */
int main() {
  enableRawMode();

  while (1) {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
      die("read");
    if (iscntrl(c)) {
      printf("%d\n\r", c);
    } else {
      printf("%d ('%c')\n\r", c, c);
    }
    if (c == 'q') {
      break;
    }
  }
  return 0;
}
