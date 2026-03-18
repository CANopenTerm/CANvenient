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

static char* lookup_error_string(TPCANStatus pcan_status);
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
        set_error_reason(lookup_error_string(pcan_status));
        return -1;
    }

    if (ch_count > CAN_MAX_INTERFACES)
    {
        ch_count = CAN_MAX_INTERFACES;
    }

    pcan_ch_info = (TPCANChannelInformation*)malloc(sizeof(TPCANChannelInformation) * ch_count);
    if (NULL == pcan_ch_info)
    {
        set_error_reason("Memory allocation failed.");
        return -1;
    }

    pcan_status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS, pcan_ch_info, sizeof(TPCANChannelInformation) * ch_count);
    if (PCAN_ERROR_OK != pcan_status)
    {
        set_error_reason(lookup_error_string(pcan_status));
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
            set_error_reason("Memory allocation failed.");
            free(pcan_ch_info);
            return -1;
        }

        name_len = strnlen(pcan_ch_info[i].device_name, sizeof(pcan_ch_info[i].device_name));
        can_interface[i].name = (char*)malloc(name_len + 1);
        if (NULL == can_interface[i].name)
        {
            set_error_reason("Memory allocation failed.");
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

    set_error_reason("No error. Success.");
    return 0;
}

int peak_open(int index)
{
#ifdef _WIN32

    TPCANHandle pcan_ch = ((TPCANChannelInformation*)can_interface[index].internal)->channel_handle;
    TPCANStatus pcan_status;

    pcan_status = CAN_Initialize(pcan_ch, can_interface[index].baudrate, 0, 0, 0);
    if (PCAN_ERROR_OK != pcan_status)
    {
        set_error_reason(lookup_error_string(pcan_status));
        return -1;
    }
    else
    {
        set_error_reason("No error. Success.");
        can_interface[index].opened = 1;
        return 0;
    }

#else
    set_error_reason("PEAK driver is only supported on Windows.");
    (void)index;
    return -1;
#endif
}

void peak_close(int index)
{
#ifdef _WIN32
    TPCANStatus pcan_status;

    pcan_status = CAN_Uninitialize(((TPCANChannelInformation*)can_interface[index].internal)->channel_handle);
    if (PCAN_ERROR_OK != pcan_status)
    {
        set_error_reason(lookup_error_string(pcan_status));
    }
    else
    {
        set_error_reason("No error. Success.");
    }

#else
    set_error_reason("PEAK driver is only supported on Windows.");
    (void)index;
#endif
}

int peak_set_baudrate(int index, enum can_baudrate baud)
{
#ifdef _WIN32
    set_error_reason("Not yet implemented.");
    (void)index;
    (void)baud;
    return -1;
#else
    set_error_reason("PEAK driver is only supported on Windows.");
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
        set_error_reason(lookup_error_string(pcan_status));
        return -1;
    }

    set_error_reason("No error. Success.");
    return 0;

#else
    set_error_reason("PEAK driver is only supported on Windows.");
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
        set_error_reason(lookup_error_string(pcan_status));
        return -1;
    }

    frame->can_id = pcan_frame.ID;
    frame->can_dlc = pcan_frame.LEN;

    *timestamp = pcan_timestamp.micros + (1000ULL * pcan_timestamp.millis) + (0x100000000ULL * 1000ULL * pcan_timestamp.millis_overflow);

    for (int i = 0; i < 8; i += 1)
    {
        frame->data[i] = pcan_frame.DATA[i];
    }

    set_error_reason("No error. Success.");
    return 0;

#else

    set_error_reason("PEAK driver is only supported on Windows.");
    (void)index;
    (void)frame;
    (void)timestamp;
    return -1;
#endif
}

#ifdef _WIN32
static char* lookup_error_string(TPCANStatus pcan_status)
{
    static char error_buf[1024];
    size_t offset = 0;
    int found = 0;
    size_t i;

    /* Some PCAN status codes share bits (e.g. ILLHW, ILLNET, ILLCLIENT all
       overlap with HWINUSE / NETINUSE).  Using a (value, mask) pair lets us
       match each code exactly without false positives. */
    static const struct
    {
        TPCANStatus value;
        TPCANStatus mask;
        const char* msg;

    } error_table[] =
        {
            {PCAN_ERROR_XMTFULL, PCAN_ERROR_XMTFULL, "Transmit buffer in CAN controller is full."},
            {PCAN_ERROR_OVERRUN, PCAN_ERROR_OVERRUN, "CAN controller was read too late."},
            {PCAN_ERROR_BUSLIGHT, PCAN_ERROR_BUSLIGHT, "Bus error: an error counter reached the 'light' limit."},
            {PCAN_ERROR_BUSHEAVY, PCAN_ERROR_BUSHEAVY, "Bus error: an error counter reached the 'heavy' limit."},
            {PCAN_ERROR_BUSPASSIVE, PCAN_ERROR_BUSPASSIVE, "Bus error: the CAN controller is error passive."},
            {PCAN_ERROR_BUSOFF, PCAN_ERROR_BUSOFF, "Bus error: the CAN controller is in bus-off state."},
            {PCAN_ERROR_QRCVEMPTY, PCAN_ERROR_QRCVEMPTY, "Receive queue is empty."},
            {PCAN_ERROR_QOVERRUN, PCAN_ERROR_QOVERRUN, "Receive queue was read too late."},
            {PCAN_ERROR_QXMTFULL, PCAN_ERROR_QXMTFULL, "Transmit queue is full."},
            {PCAN_ERROR_REGTEST, PCAN_ERROR_REGTEST, "Test of the CAN controller hardware registers failed (no hardware found)."},
            {PCAN_ERROR_NODRIVER, PCAN_ERROR_NODRIVER, "Driver not loaded."},
            /* HWINUSE (0x0400), NETINUSE (0x0800), ILLHW (0x1400), ILLNET (0x1800)
               and ILLCLIENT (0x1C00) all share bits within the 0x1C00 mask.
               Each entry is therefore matched against that common mask so that
               only the intended code is reported. */
            {PCAN_ERROR_HWINUSE, PCAN_ERROR_ILLCLIENT, "PCAN-Hardware already in use by a PCAN-Net."},
            {PCAN_ERROR_NETINUSE, PCAN_ERROR_ILLCLIENT, "A PCAN-Client is already connected to the PCAN-Net."},
            {PCAN_ERROR_ILLHW, PCAN_ERROR_ILLCLIENT, "PCAN-Hardware handle is invalid."},
            {PCAN_ERROR_ILLNET, PCAN_ERROR_ILLCLIENT, "PCAN-Net handle is invalid."},
            {PCAN_ERROR_ILLCLIENT, PCAN_ERROR_ILLCLIENT, "PCAN-Client handle is invalid."},
            {PCAN_ERROR_RESOURCE, PCAN_ERROR_RESOURCE, "Resource (FIFO, Client, timeout) cannot be created."},
            {PCAN_ERROR_ILLPARAMTYPE, PCAN_ERROR_ILLPARAMTYPE, "Invalid parameter."},
            {PCAN_ERROR_ILLPARAMVAL, PCAN_ERROR_ILLPARAMVAL, "Invalid parameter value."},
            {PCAN_ERROR_UNKNOWN, PCAN_ERROR_UNKNOWN, "Unknown error."},
            {PCAN_ERROR_ILLDATA, PCAN_ERROR_ILLDATA, "Invalid data, function, or action."},
            {PCAN_ERROR_ILLMODE, PCAN_ERROR_ILLMODE, "Driver object state is wrong for the attempted operation."},
            {PCAN_ERROR_CAUTION, PCAN_ERROR_CAUTION, "Operation succeeded but with irregularities."},
            {PCAN_ERROR_INITIALIZE, PCAN_ERROR_INITIALIZE, "Channel is not initialized."},
            {PCAN_ERROR_ILLOPERATION, PCAN_ERROR_ILLOPERATION, "Invalid operation."},
        };

    if (PCAN_ERROR_OK == pcan_status)
    {
        return "No error. Success.";
    }

    error_buf[0] = '\0';

    for (i = 0; i < sizeof(error_table) / sizeof(error_table[0]); i++)
    {
        if ((pcan_status & error_table[i].mask) == error_table[i].value)
        {
            if (found)
            {
                offset += (size_t)snprintf(error_buf + offset, sizeof(error_buf) - offset, "; ");
            }
            offset += (size_t)snprintf(error_buf + offset, sizeof(error_buf) - offset, "%s", error_table[i].msg);
            found = 1;
        }
    }

    if (! found)
    {
        return "Unknown error.";
    }

    return error_buf;
}
#endif /* _WIN32 */
