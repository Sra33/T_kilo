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
- VMIN - sets the minimum number of bytes of input needed before read() can return
- VTIME - Maximum amount of time to wait before read returns (setting it to 1, ensure 1/10 seconds)
- EAGAIN - It is often raised when performing non-blocking I/O, it indicated that a  non-blocking operation cannot be completed at the moment and the operation should be tried again later

NON-BLOCKING I/O - it is a technique in programming that allows a program to perform other task while waiting for i/o operations to complete,

BLOCKING I/O - In traditional blocking i/o the program waits for the operation to be completed before moving to the next task

FNCTL(File Control) - It is a unix system call and allows operations to be performed on a file descriptor, such operation include setting non blocking mode

### HOW DOES CONTROL KEY WORK
- The control key works by bitwise anding 0x1f with the ascii value of the character pressed. Example ctrl + a = (ox1f) & 97 it replaces the first three bit with 0


### ESCAPE SEQUENCE 
- The escape sequence /x1b]2J is used to clear the screen, where x1b is the escape character which is also 27, which is always followed by ]. J takes an argument,which are
0 - clear the screen from the cursor up to the end of the screen
1 - clears the screen from the beginning of the screen up to the cursor
2 - clears the screen

- 0 is the default argument
- Once the escape sequence is written, the screen is cleared but the curosr is usually at the bottom
- To move it to the top \x1b[H is written to the terminal,which is 3 bytes
- H takes two arguments the row and column of the cursor position, by default it is set to <esc>[1;1H, since row and column start at position 1 not 0
