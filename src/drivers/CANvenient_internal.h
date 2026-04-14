/** @file CANvenient_internal.h
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#ifndef CANVENIENT_INTERNAL_H
#define CANVENIENT_INTERNAL_H

#include "global.h"

#ifdef __cplusplus
  extern "C" {
#endif

#define VENIENT_SOFTING_DRV_DISABLED
#define VENIENT_IXXAT_DRV_DISABLED
#define VENIENT_KVASER_DRV_DISABLED
#define VENIENT_PEAK_DRV_DISABLED


/*
 * CAN interface vendor enumeration.
 */
enum can_vendor
{
    CAN_VENDOR_NONE = 0,
    CAN_VENDOR_IXXAT,
    CAN_VENDOR_KVASER,
    CAN_VENDOR_PEAK,
    CAN_VENDOR_SOCKETCAN,
    CAN_VENDOR_SOFTING,
    CAN_VENDOR_MHS
};

/*
 * CAN interface.
 */
struct can_iface
{
    u8 opened; /* 0 = closed, 1 = opened */
    char* name;
    enum can_baudrate baudrate;
    enum can_vendor vendor;
    void* internal;
};

extern struct can_iface can_interface[CAN_MAX_INTERFACES] ATTRIBUTE_INTERNAL;
extern char can_error_reason[1024] ATTRIBUTE_INTERNAL;

int find_free_interface_slot(u32* index) ATTRIBUTE_INTERNAL;
void set_error_reason(const char* reason) ATTRIBUTE_INTERNAL;

#ifdef __cplusplus
  }
#endif

#endif /* CANVENIENT_INTERNAL_H */
