/** @file CANvenient_Softing.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#ifdef _WIN32
#endif

#include "CANvenient.h"
#include "CANvenient_internal.h"

int softing_find_interfaces(void)
{
#ifdef _WIN32

#endif

    return 0;
}

int softing_open(int index)
{
#ifdef _WIN32

    set_error_reason("Softing driver is not supported yet.");
    (void)index;
    return -1;

#else
    set_error_reason("Softing driver is only supported on Windows.");
    (void)index;
    return -1;
#endif
}

void softing_close(int index)
{
#ifdef _WIN32

    set_error_reason("Softing driver is not supported yet.");
    (void)index;

#else
    set_error_reason("Softing driver is only supported on Windows.");
    (void)index;
#endif
}

int softing_set_baudrate(int index, enum can_baudrate baud)
{
#ifdef _WIN32

    set_error_reason("Softing driver is not supported yet.");
    (void)index;
    (void)baud;
    return -1;

#else
    set_error_reason("Softing driver is only supported on Windows.");
    (void)index;
    (void)baud;
    return -1;
#endif
}

int softing_send(int index, struct can_frame* frame)
{
#ifdef _WIN32

    set_error_reason("Softing driver is not supported yet.");
    (void)index;
    (void)frame;
    return -1;

#else
    set_error_reason("Softing driver is only supported on Windows.");
    (void)index;
    (void)frame;
    return -1;
#endif
}

int softing_recv(int index, struct can_frame* frame, u64* timestamp)
{
#ifdef _WIN32

    set_error_reason("Softing driver is not supported yet.");
    (void)index;
    (void)frame;
    (void)timestamp;
    return -1;

#else
    set_error_reason("Softing driver is only supported on Windows.");
    (void)index;
    (void)frame;
    (void)timestamp;
    return -1;
#endif
}
