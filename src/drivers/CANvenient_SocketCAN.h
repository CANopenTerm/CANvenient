/** @file CANvenient_SocketCAN.h
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

int socketcan_find_interfaces(void);

int socketcan_open(int index);
void socketcan_close(int index);

int socketcan_set_baudrate(int index, enum can_baudrate baud);

int socketcan_send(int index, struct can_frame* frame);
int socketcan_recv(int index, struct can_frame* frame, u64* timestamp);
