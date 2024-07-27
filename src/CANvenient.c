/** @file CANvenient.c
 *
 *  An abstraction layer for multiple CAN APIs on Windows.
 *
 *  Copyright (c) 2024, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include "CANvenient.h"

int can_init(can_handle_t** handle)
{
    return 0;
}

int can_open_channel(int channel, can_handle_t* handle)
{
    return 0;
}

int can_close_channel(int channel, can_handle_t* handle)
{
    return 0;
}

int can_send(int channel, unsigned int id, unsigned char* data, unsigned char length, can_handle_t* handle)
{
    return 0;
}

int can_receive(int channel, unsigned int* id, unsigned char* data, unsigned char* length, can_handle_t* handle)
{
    return 0;
}

int can_deinit(can_handle_t* handle)
{
    return 0;
}
