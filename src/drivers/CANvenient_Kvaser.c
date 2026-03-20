/** @file CANvenient_Kvaser.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CANvenient.h"
#include "CANvenient_internal.h"

#ifdef _WIN32
#include <windows.h>
#include <canlib.h>

typedef struct
{
    int channel;
    CanHandle handle;

} kvaser_channel_info_t;

static long baudrate_to_canlib(enum can_baudrate baud);
static const char* get_canlib_error_text(canStatus status);
#endif

int kvaser_find_interfaces(void)
{
#ifdef _WIN32

    int channel_count = 0;
    canStatus status;

    canInitializeLibrary();

    status = canGetNumberOfChannels(&channel_count);
    if (status < 0)
    {
        /* It's fine to have no channels, just return 0. */
        return 0;
    }

    if (channel_count > CAN_MAX_INTERFACES)
    {
        channel_count = CAN_MAX_INTERFACES;
    }

    for (int i = 0; i < channel_count; i++)
    {
        char device_name[256];
        kvaser_channel_info_t* ch_info;

        if (can_interface[i].name)
        {
            /* Slot already occupied: skip. */
            continue;
        }

        status = canGetChannelData(i, canCHANNELDATA_DEVDESCR_ASCII, device_name, sizeof(device_name));
        if (status < 0)
        {
            set_error_reason(get_canlib_error_text(status));
            return -1;
        }
        device_name[sizeof(device_name) - 1] = '\0';

        ch_info = (kvaser_channel_info_t*)malloc(sizeof(kvaser_channel_info_t));
        if (NULL == ch_info)
        {
            set_error_reason("Memory allocation failed.");
            return -1;
        }
        ch_info->channel = i;
        ch_info->handle = canINVALID_HANDLE;

        can_interface[i].name = (char*)malloc(strnlen(device_name, sizeof(device_name)) + 1);
        if (NULL == can_interface[i].name)
        {
            set_error_reason("Memory allocation failed.");
            free(ch_info);
            return -1;
        }
        strncpy_s(can_interface[i].name, strnlen(device_name, sizeof(device_name)) + 1, device_name, _TRUNCATE);

        can_interface[i].internal = ch_info;
        can_interface[i].vendor = CAN_VENDOR_KVASER;
        can_interface[i].opened = 0;
        can_interface[i].baudrate = CAN_BAUD_1M;
    }

#endif

    return 0;
}

int kvaser_open(int index)
{
#ifdef _WIN32

    kvaser_channel_info_t* ch_info = (kvaser_channel_info_t*)can_interface[index].internal;
    CanHandle hnd;
    canStatus status;

    hnd = canOpenChannel(ch_info->channel, 0);
    if (hnd < 0)
    {
        set_error_reason(get_canlib_error_text((canStatus)hnd));
        return -1;
    }

    status = canSetBusParams(hnd, baudrate_to_canlib(can_interface[index].baudrate), 0, 0, 0, 0, 0);
    if (status < 0)
    {
        canClose(hnd);
        set_error_reason(get_canlib_error_text(status));
        return -1;
    }

    status = canBusOn(hnd);
    if (status < 0)
    {
        canClose(hnd);
        set_error_reason(get_canlib_error_text(status));
        return -1;
    }

    ch_info->handle = hnd;
    can_interface[index].opened = 1;
    return 0;

#else
    set_error_reason("Kvaser driver is only supported on Windows.");
    (void)index;
    return -1;
#endif
}

void kvaser_close(int index)
{
#ifdef _WIN32

    kvaser_channel_info_t* ch_info = (kvaser_channel_info_t*)can_interface[index].internal;

    if (canINVALID_HANDLE == ch_info->handle)
    {
        return;
    }

    canBusOff(ch_info->handle);
    canClose(ch_info->handle);
    ch_info->handle = canINVALID_HANDLE;
    can_interface[index].opened = 0;

#else
    set_error_reason("Kvaser driver is only supported on Windows.");
    (void)index;
#endif
}

int kvaser_update(int index)
{
#ifdef _WIN32

    kvaser_channel_info_t* ch_info;
    canStatus status;
    char device_name[256];

    ch_info = (kvaser_channel_info_t*)can_interface[index].internal;
    if (NULL == ch_info)
    {
        return -1;
    }

    status = canGetChannelData(ch_info->channel, canCHANNELDATA_DEVDESCR_ASCII, device_name, sizeof(device_name));
    if (status < 0)
    {
        can_release(index);
        return -1;
    }

    return 0;

#else
    set_error_reason("Kvaser driver is only supported on Windows.");
    (void)index;
    return -1;
#endif
}

int kvaser_set_baudrate(int index, enum can_baudrate baud)
{
#ifdef _WIN32

    can_close(index);
    can_interface[index].baudrate = baud;
    return can_open(index, baud);

#else
    set_error_reason("Kvaser driver is only supported on Windows.");
    (void)index;
    (void)baud;
    return -1;
#endif
}

int kvaser_send(int index, struct can_frame* frame)
{
#ifdef _WIN32

    kvaser_channel_info_t* ch_info = (kvaser_channel_info_t*)can_interface[index].internal;
    canStatus status;

    status = canWrite(ch_info->handle, (long)frame->can_id, frame->data, frame->can_dlc, canMSG_STD);
    if (status < 0)
    {
        set_error_reason(get_canlib_error_text(status));
        return -1;
    }

    return 0;

#else
    set_error_reason("Kvaser driver is only supported on Windows.");
    (void)index;
    (void)frame;
    return -1;
#endif
}

int kvaser_recv(int index, struct can_frame* frame, u64* timestamp)
{
#ifdef _WIN32

    kvaser_channel_info_t* ch_info = (kvaser_channel_info_t*)can_interface[index].internal;
    canStatus status;
    long id;
    unsigned int dlc;
    unsigned int flag;
    unsigned long time;

    status = canRead(ch_info->handle, &id, frame->data, &dlc, &flag, &time);
    if (status < 0)
    {
        if (canERR_NOMSG != status)
        {
            set_error_reason(get_canlib_error_text(status));
        }
        return -1;
    }

    frame->can_id = (canid_t)id;
    frame->can_dlc = (u8)dlc;
    *timestamp = (u64)time * 1000ULL; /* ms to µs */

    return 0;

#else
    set_error_reason("Kvaser driver is only supported on Windows.");
    (void)index;
    (void)frame;
    (void)timestamp;
    return -1;
#endif
}

#ifdef _WIN32
static long baudrate_to_canlib(enum can_baudrate baud)
{
    switch (baud)
    {
        case CAN_BAUD_1M:
            return canBITRATE_1M;
        case CAN_BAUD_800K:
            return canBITRATE_1M; /* no 800K predefined, fall back to 1M */
        case CAN_BAUD_500K:
            return canBITRATE_500K;
        case CAN_BAUD_250K:
            return canBITRATE_250K;
        case CAN_BAUD_125K:
            return canBITRATE_125K;
        case CAN_BAUD_100K:
            return canBITRATE_100K;
        case CAN_BAUD_95K:
            return canBITRATE_83K; /* closest predefined */
        case CAN_BAUD_83K:
            return canBITRATE_83K;
        case CAN_BAUD_50K:
            return canBITRATE_50K;
        case CAN_BAUD_47K:
            return canBITRATE_50K; /* closest predefined */
        case CAN_BAUD_33K:
            return canBITRATE_62K; /* closest predefined */
        case CAN_BAUD_20K:
            return canBITRATE_10K; /* closest predefined */
        case CAN_BAUD_10K:
            return canBITRATE_10K;
        case CAN_BAUD_5K:
            return canBITRATE_10K; /* closest predefined */
        default:
            return canBITRATE_1M;
    }
}

static const char* get_canlib_error_text(canStatus status)
{
    static char error_buf[256];
    canGetErrorText(status, error_buf, (unsigned int)sizeof(error_buf));
    return error_buf;
}
#endif /* _WIN32 */
