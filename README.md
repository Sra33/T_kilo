# THIS IS THE README For the Kilo Editor

Cooked mode - by default most terminals start in this
mode, the keyboard input are sent only after the enter
button
is pressed
Raw mode - Most text editors use this mode, to set this
mode many flags are switched in the terminal
Cannon mode - It allows the terminal to read input line by line instead of byte by byte
## SETTING TERMINAL ATTRIBUTE
Terminal attributes can be set using the:
1. tcgetattr() function, to read the attributes into a struct
2. My modifying the struct by hand
3. Passing the modified struct to tcsetattr() to write new 
terminal attributes.

## ENABLING RAW MODE

- Using the termios.h header file terminal i/o interfaces can be
accessed

- tcgetattr() is used to retrieve terminal/serial port attributes, it accepts a file descriptor which specfies the serial port or terminal(STDIN_FILENO specfies the terminal)

- tcsetattr() is used to set the terminal attributes to the new one that has been changed, the TCSAFLUSH argument specfies when to apply the change, for this case it waits until all pending output has been written to the terminal, discards any input that has not been read

- ECHO is a 32 bitflag (0000 0000 0000 0000 0000 0000 0000 1000)to flip the 4th bit a common way is to (x &= ~x), not the bit then and the result

- atexit() - it is a c function that allows a function specified to be called during the exiting of a program, this function is used in other to restore the terminal setting back to its original after the program(kilo) ends

- IXON - controls software flow control keys like ctrl + s (stops data from being transmitted to the terminal), ctrl + q resumens it 
- ISIG - controls signal keys like ctrl + c, ctrl + z
