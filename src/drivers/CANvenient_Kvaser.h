/** @file CANvenient_Kvaser.h
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include "CANvenient.h"

int kvaser_find_interfaces(void);

int kvaser_open(int index);
int kvaser_open_fd(int index);
void kvaser_close(int index);
int kvaser_update(int index);

int kvaser_set_baudrate(int index, enum can_baudrate baud);

int kvaser_send(int index, const struct can_frame* frame);
int kvaser_recv(int index, struct can_frame* frame, u64* timestamp);
