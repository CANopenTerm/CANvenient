/** @file CANvenient.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on Windows,
 *  similar to SocketCAN on Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#ifndef CANVENIENT_H
#define CANVENIENT_H

#ifdef _WIN32
#define CANVENIENT_API __declspec(dllexport)

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#elif __linux__
#define CANVENIENT_API __attribute__((visibility("default")))

#include <linux/types.h>
#include <linux/can.h>

typedef __u8 u8;
typedef __u16 u16;
typedef __u32 u32;
typedef __u64 u64;

#endif

/*
 * CAN interface vendor enumeration.
 */
enum can_vendor
{
    CAN_VENDOR_SOCKETCAN = 0,
    CAN_VENDOR_PEAK = 1,
    CAN_VENDOR_IXXAT = 2
};

/*
 * CAN interface baud rate enumeration.
 */
enum can_baudrate
{
    CAN_BAUD_1M = 0,
    CAN_BAUD_800K,
    CAN_BAUD_500K,
    CAN_BAUD_250K,
    CAN_BAUD_125K,
    CAN_BAUD_100K,
    CAN_BAUD_95K,
    CAN_BAUD_83K,
    CAN_BAUD_50K,
    CAN_BAUD_47K,
    CAN_BAUD_33K,
    CAN_BAUD_20K,
    CAN_BAUD_10K,
    CAN_BAUD_5K
};

/*
 * CAN interface.
 */
struct can_iface
{
    u32 id;
    u8 opened; /* 0 = closed, 1 = opened */
    char* name;
    enum can_baudrate baudrate;
    enum can_vendor vendor;
};

#ifndef _CAN_H
#define _CAN_H

#define CAN_MAX_DLEN 8

/*
 * CAN message frame.
 */
struct can_frame
{
    u64 timestamp; /* Timestamp in microseconds */
    u32 can_id;    /* 32 bit CAN_ID + EFF/RTR/ERR flags */
    u8 pad;        /* padding */
    u8 res0;       /* reserved / padding */
    u8 len8_dlc;   /* optional DLC for 8 byte payload length (9 .. 15) */
    u8 data[CAN_MAX_DLEN];
};

#endif /* _CAN_H */

CANVENIENT_API int can_find_interfaces(struct can_iface* iface[], int* count);
CANVENIENT_API void can_free_interfaces(struct can_iface* iface[], int count);

CANVENIENT_API int can_open(struct can_iface* iface);
CANVENIENT_API int can_open_fd(struct can_iface* iface);
CANVENIENT_API void can_close(struct can_iface* iface);

CANVENIENT_API int can_send(struct can_iface* iface, struct can_frame* frame);
CANVENIENT_API int can_recv(struct can_iface* iface, struct can_frame* frame);

#endif /* CANVENIENT_H */
