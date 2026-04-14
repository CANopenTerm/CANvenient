/** @file example.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <CANvenient.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
  #include <conio.h>
#else
  #include "linux_util.h"
#endif


#ifdef _WIN32
  #define UtilInit()
  #define KeyHit kbhit
#else
  #include <unistd.h>
  #define getch getchar
#endif


static int OpenInterface(int *index)
{
int res;

res = 0;
if (!index)
  return(-1);
if (!can_find_interfaces())
  {
  for (int i = 0; i < CAN_MAX_INTERFACES; i++)
    {
    char name[256] = {0};
    can_get_name(i, name, sizeof(name));
    if (name[0] != '\0')
      {
      if (0 == can_open(i, CAN_BAUD_1M))
        {
        printf("Opened CAN interface: %s\n\r", name);
        res = 1;
        *index = i;
        break;
        }
      else
        {
        char error_reason[256] = {0};
        can_get_error(error_reason, sizeof(error_reason));
        printf("Failed to open CAN interface %s: %s\n\r", name, error_reason);
        }
      }
    }
  }
return(res);
}


static void PrintCanFrame(struct can_frame* frame, u64 timestamp)
{
uint8_t i;
(void)timestamp;

printf("id:%03X dlc:%0d data:", frame->can_id, frame->can_dlc);
if (frame->can_dlc)
  {
  for (i = 0; i < frame->can_dlc; i++)
    printf("%02X ", frame->data[i]);
  }
else
  printf(" no data");
printf("\n\r");
}


static void TxStdFrame(int index, uint32_t id, uint8_t len, const uint8_t *data)
{
struct can_frame frame;

frame.can_id = id;
frame.can_dlc = len;
if ((len > 0) && (len <= 8))
  memcpy(frame.data, data, len);
(void)can_send(index, &frame);
}


int main()
{
int index;
char ch;
struct can_frame rx_frame;
u64 timestamp;

UtilInit();
if (OpenInterface(&index) >= 1)
  {
  ch = '\0';
  printf("press \"t\" to send test message and \"q\" for quit\n\r\n\r");
  printf("received messages:\n\r");
  do
    {
    if (can_recv(index, &rx_frame, &timestamp) == 0)
      PrintCanFrame(&rx_frame, timestamp);
    if (KeyHit())
      {
      ch = getch();
      switch (ch)
        {
        case 't' : {
                   TxStdFrame(index, 123, 8, (const uint8_t *)"*HALLO*");
                   break;
                   }
        }
      }
    }
  while (ch != 'q');
  }
/* Implicitly calls can_close() for all opened interfaces. */
can_release_interfaces();
return(EXIT_SUCCESS);
}
