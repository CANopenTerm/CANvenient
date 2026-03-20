/** @file CANvenient_template.h
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include "CANvenient.h"

int template_find_interfaces(void);

int template_open(int index);
int template_open_fd(int index);
void template_close(int index);
int template_update(int index);

int template_set_baudrate(int index, enum can_baudrate baud);

int template_send(int index, struct can_frame* frame);
int template_recv(int index, struct can_frame* frame, u64* timestamp);
