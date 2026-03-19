/** @file CANvenient_Softing.h
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include "CANvenient.h"

int softing_find_interfaces(void);

int softing_open(int index);
int softing_open_fd(int index);
void softing_close(int index);

int softing_set_baudrate(int index, enum can_baudrate baud);

int softing_send(int index, struct can_frame* frame);
int softing_recv(int index, struct can_frame* frame, u64* timestamp);
