/** @file CANvenient_PEAK.h
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

int peak_find_interfaces(void);

int peak_open(int index);
int peak_open_fd(int index);
void peak_close(int index);

int peak_set_baudrate(int index, enum can_baudrate baud);

int peak_send(int index, struct can_frame* frame);
int peak_recv(int index, struct can_frame* frame, u64* timestamp);
