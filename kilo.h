#ifndef KILO_H_
#define KILO_H_
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

void disableRawMode();
void enableRawMode();
#endif // KILO_H_
