/** @file CANvenient_Softing.h
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

int softing_find_interfaces(void) ATTRIBUTE_INTERNAL;

int softing_open(int index) ATTRIBUTE_INTERNAL;
int softing_open_fd(int index) ATTRIBUTE_INTERNAL;
void softing_close(int index) ATTRIBUTE_INTERNAL;
int softing_update(int index) ATTRIBUTE_INTERNAL;

int softing_set_baudrate(int index, enum can_baudrate baud) ATTRIBUTE_INTERNAL;

int softing_send(int index, struct can_frame* frame) ATTRIBUTE_INTERNAL;
int softing_recv(int index, struct can_frame* frame, u64* timestamp) ATTRIBUTE_INTERNAL;

#ifdef __cplusplus
  }
#endif