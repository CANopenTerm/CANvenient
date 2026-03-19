/** @file CANvenient.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <stdio.h>
#include <stdlib.h>

#include "CANvenient.h"
#include "drivers/CANvenient_internal.h"
#include "drivers/CANvenient_Ixxat.h"
#include "drivers/CANvenient_Kvaser.h"
#include "drivers/CANvenient_SocketCAN.h"
#include "drivers/CANvenient_Softing.h"
#include "drivers/CANvenient_PEAK.h"

struct can_iface can_interface[CAN_MAX_INTERFACES] = {0};
char can_error_reason[1024] = {0};

CANVENIENT_API int can_find_interfaces(void)
{
    int status;

        status = ixxat_find_interfaces();
    if (status < 0)
    {
        return status;
    }

    status = peak_find_interfaces();
    if (status < 0)
    {
        return status;
    }

    status = kvaser_find_interfaces();
    if (status < 0)
    {
        return status;
    }

    status = socketcan_find_interfaces();
    if (status < 0)
    {
        return status;
    }

    return softing_find_interfaces();
}

CANVENIENT_API void can_free_interfaces(void)
{
    for (int i = 0; i < CAN_MAX_INTERFACES; i++)
    {
        can_close(i);
    }
}

CANVENIENT_API int can_open(int index)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return -1;
    }

    switch (can_interface[index].vendor)
    {
        case CAN_VENDOR_IXXAT:
            return ixxat_open(index);
        case CAN_VENDOR_KVASER:
            return kvaser_open(index);
        case CAN_VENDOR_PEAK:
            return peak_open(index);
        case CAN_VENDOR_SOCKETCAN:
            return socketcan_open(index);
        case CAN_VENDOR_SOFTING:
            return softing_open(index);
        default:
        case CAN_VENDOR_NONE:
            return -1;
    }
}

CANVENIENT_API void can_close(int index)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return;
    }

    switch (can_interface[index].vendor)
    {
        case CAN_VENDOR_IXXAT:
        {
            ixxat_close(index);
            break;
        }
        case CAN_VENDOR_KVASER:
            kvaser_close(index);
            break;
        case CAN_VENDOR_PEAK:
            peak_close(index);
            break;
        case CAN_VENDOR_SOCKETCAN:
            socketcan_close(index);
            break;
        case CAN_VENDOR_SOFTING:
            softing_close(index);
            break;
        default:
        case CAN_VENDOR_NONE:
            break;
    }

    can_interface[index].vendor = CAN_VENDOR_NONE;
    can_interface[index].opened = 0;

    if (can_interface[index].name)
    {
        free(can_interface[index].name);
        can_interface[index].name = NULL;
    }
    if (can_interface[index].internal)
    {
        free(can_interface[index].internal);
        can_interface[index].internal = NULL;
    }
}

CANVENIENT_API void can_get_error_reason(char* reason_buf, size_t buf_size)
{
    if (NULL == reason_buf || buf_size == 0)
    {
        return;
    }
    snprintf(reason_buf, buf_size, "%s", can_error_reason);
}

CANVENIENT_API int can_get_name(int index, char* name_buf, size_t buf_size)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return -1;
    }
    else if (NULL == can_interface[index].name)
    {
        return -1;
    }
    else if (NULL == name_buf)
    {
        return -1;
    }
    else
    {
        snprintf(name_buf, buf_size, "%s", can_interface[index].name);
        return 0;
    }
}

CANVENIENT_API int can_set_baudrate(int index, enum can_baudrate baud)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return -1;
    }
    else if (baud < CAN_BAUD_1M || baud > CAN_BAUD_5K)
    {
        return -1;
    }
    else if (! can_interface[index].name)
    {
        return -1;
    }

    switch (can_interface[index].vendor)
    {
        case CAN_VENDOR_IXXAT:
            return ixxat_set_baudrate(index, baud);
        case CAN_VENDOR_KVASER:
            return kvaser_set_baudrate(index, baud);
        case CAN_VENDOR_PEAK:
            return peak_set_baudrate(index, baud);
        case CAN_VENDOR_SOCKETCAN:
            return socketcan_set_baudrate(index, baud);
        case CAN_VENDOR_SOFTING:
            return softing_set_baudrate(index, baud);
        default:
        case CAN_VENDOR_NONE:
            return -1;
    }
}

CANVENIENT_API int can_send(int index, struct can_frame* frame)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return -1;
    }

    switch (can_interface[index].vendor)
    {
        case CAN_VENDOR_IXXAT:
            return ixxat_send(index, frame);
        case CAN_VENDOR_KVASER:
            return kvaser_send(index, frame);
        case CAN_VENDOR_PEAK:
            return peak_send(index, frame);
        case CAN_VENDOR_SOCKETCAN:
            return socketcan_send(index, frame);
        case CAN_VENDOR_SOFTING:
            return softing_send(index, frame);
        default:
        case CAN_VENDOR_NONE:
            return -1;
    }
}

CANVENIENT_API int can_recv(int index, struct can_frame* frame, u64* timestamp)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return -1;
    }

    switch (can_interface[index].vendor)
    {
        case CAN_VENDOR_IXXAT:
            return ixxat_recv(index, frame, timestamp);
        case CAN_VENDOR_KVASER:
            return kvaser_recv(index, frame, timestamp);
        case CAN_VENDOR_PEAK:
            return peak_recv(index, frame, timestamp);
        case CAN_VENDOR_SOCKETCAN:
            return socketcan_recv(index, frame, timestamp);
        case CAN_VENDOR_SOFTING:
            return softing_recv(index, frame, timestamp);
        default:
        case CAN_VENDOR_NONE:
            return -1;
    }
}

int find_free_interface_slot(u32* index)
{
    for (u32 i = 0; i < CAN_MAX_INTERFACES; i++)
    {
        if (NULL == can_interface[i].name)
        {
            *index = i;
            return 0;
        }
    }
    *index = CAN_MAX_INTERFACES; /* No free slot found */
    return -1;
}

void set_error_reason(const char* reason)
{
    snprintf(can_error_reason, sizeof(can_error_reason), "%s", reason);
}
