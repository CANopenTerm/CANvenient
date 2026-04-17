/** @file CANvenient_Ixxat.h
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include "CANvenient.h"

int ixxat_find_interfaces(void);

int ixxat_open(int index);
void ixxat_close(int index);
int ixxat_update(int index);

int ixxat_set_baudrate(int index, enum can_baudrate baud);

int ixxat_send(int index, struct can_frame* frame);
int ixxat_recv(int index, struct can_frame* frame, u64* timestamp);
