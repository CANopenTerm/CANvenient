/** @file CANvenient_template.c
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

int template_find_interfaces(void)
{
#ifdef _WIN32

#endif

    return 0;
}

int template_open(int index)
{
#ifdef _WIN32

    set_error_reason("template driver is not supported yet.");
    (void)index;
    return -1;

#else
    set_error_reason("template driver is only supported on Windows.");
    (void)index;
    return -1;
#endif
}

void template_close(int index)
{
#ifdef _WIN32

    set_error_reason("template driver is not supported yet.");
    (void)index;

#else
    set_error_reason("template driver is only supported on Windows.");
    (void)index;
#endif
}

int template_set_baudrate(int index, enum can_baudrate baud)
{
#ifdef _WIN32

    set_error_reason("template driver is not supported yet.");
    (void)index;
    (void)baud;
    return -1;

#else
    set_error_reason("template driver is only supported on Windows.");
    (void)index;
    (void)baud;
    return -1;
#endif
}

int template_send(int index, struct can_frame* frame)
{
#ifdef _WIN32

    set_error_reason("template driver is not supported yet.");
    (void)index;
    (void)frame;
    return -1;

#else
    set_error_reason("template driver is only supported on Windows.");
    (void)index;
    (void)frame;
    return -1;
#endif
}

int template_recv(int index, struct can_frame* frame, u64* timestamp)
{
#ifdef _WIN32

    set_error_reason("template driver is not supported yet.");
    (void)index;
    (void)frame;
    (void)timestamp;
    return -1;

#else
    set_error_reason("template driver is only supported on Windows.");
    (void)index;
    (void)frame;
    (void)timestamp;
    return -1;
#endif
}
