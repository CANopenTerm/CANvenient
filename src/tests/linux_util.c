/*******************************************************************************
                        linux_util.c  -  description
                             -------------------
    begin             : 12.04.2026
    last modify       : 12.04.2026
    copyright         : (C) 2026 by MHS-Elektronik GmbH & Co. KG, Germany
                               http://www.mhs-elektronik.de
    author            : Klaus Demlehner, klaus@mhs-elektronik.de
 *******************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software, you can redistribute it and/or modify  *
 *   it under the terms of the MIT License <LICENSE.TXT or                 *
 *   http://opensource.org/licenses/MIT>                                   *              
 *                                                                         *
 ***************************************************************************/ 
#ifndef _WIN32
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "linux_util.h"



#ifndef STDIN_FILENO
  #define STDIN_FILENO 0
#endif


static struct termios OldTermattr;


void UtilCleanup(void);


int UtilInit(void)
{
struct termios termattr;

if (tcgetattr(STDIN_FILENO, &OldTermattr) < 0)
  return(-1);

memcpy(&termattr, &OldTermattr, sizeof(struct termios));

termattr.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
termattr.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
termattr.c_cflag &= ~(CSIZE | PARENB);
termattr.c_cflag |= CS8;
termattr.c_oflag &= ~(OPOST);

termattr.c_cc[VMIN] = 1;  /* or 0 for some Unices;  see note 1 */
termattr.c_cc[VTIME] = 0;

if (tcsetattr (STDIN_FILENO, TCSANOW, &termattr) < 0)
  return(-1);
return(atexit(UtilCleanup));
}


void UtilCleanup(void)
{
tcsetattr(STDIN_FILENO, TCSAFLUSH, &OldTermattr);
}



int KeyHit(void)
{
int bytes_waiting;

ioctl(STDIN_FILENO, FIONREAD, &bytes_waiting);
return(bytes_waiting);
}


/*char getch(void)
{
char buf;

if (read(STDIN_FILENO, &buf, 1) != 1)
  return('\0');
else  
  return(buf);
} */

#endif


