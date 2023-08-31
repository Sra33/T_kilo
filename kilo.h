#ifndef KILO_H_
#define KILO_H_
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k)&0x1f)
void disableRawMode();
void enableRawMode();
int getCursorPosition(int *, int *);
#endif // KILO_H_
