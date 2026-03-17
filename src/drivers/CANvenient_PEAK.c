/** @file CANvenient_PEAK.c
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
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <PCANBasic.h>
#endif

#include "CANvenient.h"
#include "CANvenient_internal.h"

int peak_find_interfaces(void)
{
#ifdef _WIN32

    u32 ch_count = 0;
    TPCANChannelInformation* pcan_ch_info;
    TPCANStatus pcan_status;

    /* PEAK-System. */
    pcan_status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS_COUNT, &ch_count, sizeof(u32));
    if (PCAN_ERROR_OK != pcan_status)
    {
        return -1;
    }

    if (ch_count > CAN_MAX_INTERFACES)
    {
        ch_count = CAN_MAX_INTERFACES;
    }

    pcan_ch_info = (TPCANChannelInformation*)malloc(sizeof(TPCANChannelInformation) * ch_count);
    if (NULL == pcan_ch_info)
    {
        return -1;
    }

    pcan_status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS, pcan_ch_info, sizeof(TPCANChannelInformation) * ch_count);
    if (PCAN_ERROR_OK != pcan_status)
    {
        free(pcan_ch_info);
        pcan_ch_info = NULL;
        return -1;
    }

    for (u32 i = 0; i < ch_count; i++)
    {
        size_t name_len;

        if (can_interface[i].name)
        {
            /* Slot already occupied: skip. */
            i++;
            continue;
        }

        can_interface[i].internal = malloc(sizeof(TPCANChannelInformation));
        if (! can_interface[i].internal)
        {
            free(pcan_ch_info);
            return -1;
        }

        name_len = strnlen(pcan_ch_info[i].device_name, sizeof(pcan_ch_info[i].device_name));
        can_interface[i].name = (char*)malloc(name_len + 1);
        if (NULL == can_interface[i].name)
        {
            for (u32 j = 0; j < i; j++)
            {
                free(can_interface[j].name);
            }
            free(pcan_ch_info);
            return -1;
        }

        /* Set interface properties */
        snprintf(can_interface[i].name, name_len + 1, "%.*s", (int)name_len, pcan_ch_info[i].device_name);
        memcpy(can_interface[i].internal, &pcan_ch_info[i], sizeof(TPCANChannelInformation));

        can_interface[i].vendor = CAN_VENDOR_PEAK;
        can_interface[i].opened = 0;
        can_interface[i].baudrate = CAN_BAUD_1M;
    }
    free(pcan_ch_info);

#endif

    return 0;
}

int peak_open(int index)
{
#ifdef _WIN32

    TPCANHandle pcan_ch = ((TPCANChannelInformation*)can_interface[index].internal)->channel_handle;
    TPCANStatus pcan_status;

    pcan_status = CAN_Initialize(
        pcan_ch,
        can_interface[index].baudrate, 0, 0, 0);

    if (PCAN_ERROR_OK != pcan_status)
    {
        return -1;
    }
    else
    {
        can_interface[index].opened = 1;
        return 0;
    }

#else
    (void)index;
    return -1;
#endif
}

void peak_close(int index)
{
#ifdef _WIN32

    CAN_Uninitialize(((TPCANChannelInformation*)can_interface[index].internal)->channel_handle);

#else
    (void)index;
#endif
}

int peak_set_baudrate(int index, enum can_baudrate baud)
{
#ifdef _WIN32

#else
    (void)index;
    (void)baud;
    return -1;
#endif
}

int peak_send(int index, struct can_frame* frame)
{
#ifdef _WIN32

    TPCANHandle pcan_ch = ((TPCANChannelInformation*)can_interface[index].internal)->channel_handle;
    TPCANStatus pcan_status;
    TPCANMsg pcan_frame = {0};

    pcan_frame.ID = frame->can_id;
    pcan_frame.LEN = frame->can_dlc;

    /* pcan_frame.MSGTYPE = PCAN_MESSAGE_EXTENDED; */
    pcan_frame.MSGTYPE = PCAN_MESSAGE_STANDARD;

    for (int i = 0; i < 8; i += 1)
    {
        pcan_frame.DATA[i] = frame->data[i];
    }

    pcan_status = CAN_Write(pcan_ch, &pcan_frame);

    if (PCAN_ERROR_OK != pcan_status)
    {
        return -1;
    }

    return 0;

#else
    (void)index;
    (void)frame;
    return -1;
#endif
}

int peak_recv(int index, struct can_frame* frame, u64* timestamp)
{
#ifdef _WIN32

    TPCANHandle pcan_ch = ((TPCANChannelInformation*)can_interface[index].internal)->channel_handle;
    TPCANStatus pcan_status;
    TPCANMsg pcan_frame = {0};
    TPCANTimestamp pcan_timestamp = {0};

    pcan_status = CAN_Read(pcan_ch, &pcan_frame, &pcan_timestamp);
    if (PCAN_ERROR_OK != pcan_status)
    {
        return -1;
    }

    frame->can_id = pcan_frame.ID;
    frame->can_dlc = pcan_frame.LEN;

    *timestamp = pcan_timestamp.micros + (1000ULL * pcan_timestamp.millis) + (0x100000000ULL * 1000ULL * pcan_timestamp.millis_overflow);

    for (int i = 0; i < 8; i += 1)
    {
        frame->data[i] = pcan_frame.DATA[i];
    }

    return 0;

#else
    (void)index;
    (void)frame;
    (void)timestamp;
    return -1;
#endif
}
