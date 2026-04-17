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

extern struct can_iface can_interface[CAN_MAX_INTERFACES];
extern char can_error_reason[1024];

int find_free_interface_slot(u32* index);
void set_error_reason(const char* reason);

#endif /* CANVENIENT_INTERNAL_H */
