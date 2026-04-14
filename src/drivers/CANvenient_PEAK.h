/** @file CANvenient_PEAK.h
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/
#include "global.h"
#include "CANvenient.h"

#ifdef __cplusplus
  extern "C" {
#endif

int peak_find_interfaces(void) ATTRIBUTE_INTERNAL;

int peak_open(int index) ATTRIBUTE_INTERNAL;
int peak_open_fd(int index) ATTRIBUTE_INTERNAL;
void peak_close(int index) ATTRIBUTE_INTERNAL;
int peak_update(int index) ATTRIBUTE_INTERNAL;

int peak_set_baudrate(int index, enum can_baudrate baud) ATTRIBUTE_INTERNAL;

int peak_send(int index, struct can_frame* frame) ATTRIBUTE_INTERNAL;
int peak_recv(int index, struct can_frame* frame, u64* timestamp) ATTRIBUTE_INTERNAL;

#ifdef __cplusplus
  }
#endif