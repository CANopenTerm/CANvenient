/** @file CANvenient.h
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#ifndef CANVENIENT_H
#define CANVENIENT_H

#include <stddef.h>

#define CAN_MAX_INTERFACES 64

#ifdef _WIN32
#define CANVENIENT_API __declspec(dllexport)

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint32_t canid_t;

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

#ifndef _CAN_H
#define _CAN_H

#define CAN_MAX_DLEN 8

#ifdef _WIN32

/*
 * CAN message frame.
 *
 * This is a drop-in replacement for the Linux can_frame struct defined in linux/can.h.
 */
#pragma pack(push, 1)
struct can_frame
{
    canid_t can_id;
    union
    {
        u8 len;
        u8 can_dlc;
    };

    u8 __pad;
    u8 __res0;
    u8 len8_dlc;

    __declspec(align(8)) u8 data[CAN_MAX_DLEN];
};
#pragma pack(pop)

#endif

#endif /* _CAN_H */

CANVENIENT_API int can_find_interfaces(void);
CANVENIENT_API void can_free_interfaces(void);

CANVENIENT_API int can_open(int index);
CANVENIENT_API void can_close(int index);

CANVENIENT_API void can_get_error_reason(char* reason_buf, size_t buf_size);
CANVENIENT_API int can_get_name(int index, char* name_buf, size_t buf_size);

CANVENIENT_API int can_set_baudrate(int index, enum can_baudrate baud);

CANVENIENT_API int can_send(int index, struct can_frame* frame);
CANVENIENT_API int can_recv(int index, struct can_frame* frame, u64* timestamp);

#endif /* CANVENIENT_H */
