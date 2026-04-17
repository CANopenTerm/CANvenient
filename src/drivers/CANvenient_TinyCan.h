/** @file CANvenient_TinyCan.h
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include "CANvenient.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int tinycan_find_interfaces(void);

    int tinycan_open(int index);
    void tinycan_close(int index);
    int tinycan_update(int index);

    int tinycan_set_baudrate(int index, enum can_baudrate baud);

    int tinycan_send(int index, const struct can_frame* frame);
    int tinycan_recv(int index, struct can_frame* frame, u64* timestamp);

#ifdef __cplusplus
}
#endif
