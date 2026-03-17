/** @file CANvenient_Ixxat.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include "CANvenient.h"
#include "CANvenient_internal.h"

#ifdef _WIN32

#endif

int ixxat_find_interfaces(void)
{
#ifdef _WIN32

#else
    return 0;
#endif
}

int ixxat_open(int index)
{
#ifdef _WIN32

#else
    (void)index;
    return -1;
#endif
}

void ixxat_close(int index)
{
#ifdef _WIN32

#else
    (void)index;
#endif
}

int ixxat_set_baudrate(int index, enum can_baudrate baud)
{
#ifdef _WIN32

#else
    (void)index;
    (void)baud;
    return -1;
#endif
}

int ixxat_send(int index, struct can_frame* frame)
{
#ifdef _WIN32

#else
    (void)index;
    (void)frame;
    return -1;
#endif
}

int ixxat_recv(int index, struct can_frame* frame, u64* timestamp)
{
#ifdef _WIN32

#else
    (void)index;
    (void)frame;
    (void)timestamp;
    return -1;
#endif
}
