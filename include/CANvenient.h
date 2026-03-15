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

#include <limits.h>

#if UCHAR_MAX == 0xffU
typedef unsigned char u8;
#else
#error "No 8-bit unsigned type available"
#endif

#if USHRT_MAX == 0xffffU
typedef unsigned short u16;
#elif UINT_MAX == 0xffffU
typedef unsigned int u16;
#else
#error "No 16-bit unsigned type available"
#endif

#if UINT_MAX == 0xffffffffU
typedef unsigned int u32;
#elif ULONG_MAX == 0xffffffffUL
typedef unsigned long u32;
#elif USHRT_MAX == 0xffffffffU
typedef unsigned short u32;
#else
#error "No 32-bit unsigned type available"
#endif

#if ULONG_MAX == 0xffffffffffffffffUL
typedef unsigned long u64;
#elif defined(_WIN32) && defined(_MSC_VER)
typedef unsigned __int64 u64;
#else
typedef unsigned long long u64;
#endif

#elif __linux__
#define CANVENIENT_API __attribute__((visibility("default")))

#include <linux/types.h>

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
    CAN_VENDOR_PEAK = 0,
    CAN_VENDOR_IXXAT = 1,
    CAN_VENDOR_KVASER = 2
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

/*
 * CAN message frame.
 */
struct can_frame
{
    u64 timestamp;  /* Timestamp in microseconds */
    u32 id;         /* 11 or 29 bit identifier */
    u8 flags;       /* CAN frame flags (FD, BRS, ESI, RTR, etc.) */
    u8 dlc;         /* Data length code: number of bytes of data (0..8 for CAN, 0..64 for CAN FD) */
    u8 data[64];    /* CAN frame payload (0..8 bytes for CAN, 0..64 bytes for CAN FD) */
    u8 reserved[2]; /* Padding for alignment */
};

CANVENIENT_API int can_find_interfaces(struct can_iface* iface[], int* count);
CANVENIENT_API void can_free_interfaces(struct can_iface* iface[], int count);

CANVENIENT_API int can_open(struct can_iface* iface);
CANVENIENT_API int can_open_fd(struct can_iface* iface);
CANVENIENT_API void can_close(struct can_iface* iface);

CANVENIENT_API int can_send(struct can_iface* iface, struct can_frame* frame);
CANVENIENT_API int can_recv(struct can_iface* iface, struct can_frame* frame);

#endif /* CANVENIENT_H */
