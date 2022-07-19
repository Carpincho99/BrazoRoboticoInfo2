#include "../inc/ttyConfig.h"
#include <stdio.h>
#include <termios.h>

int main(void) {
  char puerto[] = "/dev/ttyACM0";

  int fd = open(puerto, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
  if (fd == -1) {
    return -1;
  }

  fcntl(fd, F_SETFL, 0);
  ttySet(fd, 115200, &tty);

  return 0;
}

int ttySet(int fd, int baudrate, struct termios* tty) {
  switch (baudrate) {
    case 115200:
      baudrate = B115200;
      break;
    case 57600:
      baudrate = B57600;
      break;
    case 38400:
      baudrate = B38400;
      break;
    case 19200:
      baudrate = B19200;
      break;
    case 9600:
      baudrate = B9600;
      break;
    default:
      baudrate = B115200;
      break;
  }

  if (tcgetattr(fd, tty) != 0) {
    printf("ERROR: tcgetattr\n");
    return -1;
  }

  cfsetospeed(tty, baudrate);
  cfsetispeed(tty, baudrate);

  tty->c_cflag = (tty->c_cflag & ~CSIZE) | CS8; /* 8 data bits (8) */
  tty->c_cflag &= ~(PARENB | PARODD);           /* no parity   (N) */
  tty->c_cflag &= ~CSTOPB;                      /* 1 stop bit  (1) */
  tty->c_cflag |= (CLOCAL | CREAD); /* ignore modem status lines, and */
                                    /* enable reading */

  /* Input flags - Turn off input processing
   * convert break to null byte, no CR to NL translation,
   * no NL to CR translation, don't mark parity errors or breaks
   * no input parity check, don't strip high bit off,
   * no XON/XOFF software flow control */
  tty->c_iflag &=
      ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);

  /* Output flags - Turn off output processing
   * no CR to NL translation, no NL to CR-NL translation,
   * no NL to CR translation, no column 0 CR suppression,
   * no Ctrl-D suppression, no fill characters, no case mapping,
   * no local output processing */
  tty->c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OPOST);

  /* No line processing:
   * echo off, echo newline off, canonical mode off,
   * extended input processing off, signal chars off */
  tty->c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

  /* One input byte is enough to return from read()
   * Inter-character timer off */
  tty->c_cc[VMIN] = 1;
  tty->c_cc[VTIME] = 0;

  /*
   * TCSANOW: Make the change immediately.
   * TCSADRAIN: Make the change after waiting until all queued output has been
   * written. TCSAFLUSH: This is like TCSADRAIN, but also discards any queued
   * input.
   */
  if (tcsetattr(fd, TCSAFLUSH, tty) != 0) {
    printf("ERROR: tcsetattr\n");
    return -1;
  }

  return 0;
}
