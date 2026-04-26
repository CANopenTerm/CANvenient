/** @file CANvenient.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "CANvenient.h"
#include "drivers/CANvenient_internal.h"
#include "drivers/CANvenient_Ixxat.h"
#include "drivers/CANvenient_Kvaser.h"
#include "drivers/CANvenient_SocketCAN.h"
#include "drivers/CANvenient_Softing.h"
#include "drivers/CANvenient_PEAK.h"

#ifdef _WIN32
static HMODULE vciapi_dll_handle = NULL;
static bool is_vciapi_dll_available = false;
#endif

struct can_iface can_interface[CAN_MAX_INTERFACES] = {0};
char can_error_reason[1024] = {0};

CANVENIENT_API int can_find_interfaces_mask(u32 vendor_mask)
{
    if (vendor_mask == CAN_VENDOR_NONE)
    {
        return 0;
    }

#ifdef _WIN32
    static bool once_checked_vciapi_dll = false;

    // https://github.com/CANopenTerm/CANopenTerm/issues/109#issuecomment-4320171025
    if (! once_checked_vciapi_dll)
    {
        static const char* vciapi_search_paths[] = {
            "vciapi.dll",
            "C:\\Program Files\\HMS\\Ixxat VCI\\sdk\\vci\\bin\\x64\\release\\vciapi.dll",
            NULL
        };

        for (int p = 0; vciapi_search_paths[p] != NULL; p++)
        {
            if (GetFileAttributesA(vciapi_search_paths[p]) != INVALID_FILE_ATTRIBUTES)
            {
                vciapi_dll_handle = LoadLibraryA(vciapi_search_paths[p]);
                if (vciapi_dll_handle)
                {
                    is_vciapi_dll_available = true;
                    break;
                }
            }
        }

        if (! is_vciapi_dll_available)
        {
            puts("vciapi.dll not found. Ixxat interfaces will be ignored.");
        }

        once_checked_vciapi_dll = true;
    }
    if (vendor_mask & CAN_VENDOR_IXXAT && is_vciapi_dll_available)
    {
        ixxat_find_interfaces();
    }

    if (vendor_mask & CAN_VENDOR_PEAK)
    {
        peak_find_interfaces();
    }
    if (vendor_mask & CAN_VENDOR_KVASER)
    {
        kvaser_find_interfaces();
    }

    if (vendor_mask & CAN_VENDOR_SOFTING)
    {
        softing_find_interfaces();
    }
#endif

    if (vendor_mask & CAN_VENDOR_SOCKETCAN)
    {
        socketcan_find_interfaces();
    }

    return 0;
}

CANVENIENT_API int can_find_interfaces(void)
{
    return can_find_interfaces_mask(CAN_VENDOR_ALL);
}

CANVENIENT_API void can_release_interfaces(void)
{
    for (int i = 0; i < CAN_MAX_INTERFACES; i++)
    {
        can_release(i);
    }
}

CANVENIENT_API int can_open(int index, enum can_baudrate baud)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        set_error_reason("Channel index is out-of-range.");
        return -1;
    }

    can_interface[index].baudrate = baud;

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
            set_error_reason("No CAN interface found at specified index.");
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
}

CANVENIENT_API void can_release(int index)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return;
    }

    can_close(index);

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

CANVENIENT_API int can_update(int index)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        set_error_reason("Channel index is out-of-range.");
        return -1;
    }

    switch (can_interface[index].vendor)
    {
        case CAN_VENDOR_IXXAT:
            return ixxat_update(index);
        case CAN_VENDOR_KVASER:
            return kvaser_update(index);
        case CAN_VENDOR_PEAK:
            return peak_update(index);
        case CAN_VENDOR_SOCKETCAN:
            return socketcan_update(index);
        case CAN_VENDOR_SOFTING:
            return softing_update(index);
        default:
        case CAN_VENDOR_NONE:
            set_error_reason("No CAN interface found at specified index.");
            return -1;
    }
}

CANVENIENT_API int can_get_baudrate(int index, enum can_baudrate* baud)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        set_error_reason("Channel index is out-of-range.");
        return -1;
    }
    else if (! baud)
    {
        set_error_reason("Output parameter is NULL.");
        return -1;
    }
    else
    {
        *baud = can_interface[index].baudrate;
        return 0;
    }
}

CANVENIENT_API void can_get_error(char* reason_buf, size_t buf_size)
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
        set_error_reason("Channel index is out-of-range.");
        return -1;
    }
    else if (NULL == can_interface[index].name)
    {
        set_error_reason("CAN interface name is not set.");
        return -1;
    }
    else if (NULL == name_buf)
    {
        set_error_reason("Output parameter is NULL.");
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
        set_error_reason("Channel index is out-of-range.");
        return -1;
    }
    else if (baud < CAN_BAUD_1M || baud > CAN_BAUD_5K)
    {
        set_error_reason("Invalid baudrate specified.");
        return -1;
    }
    else if (! can_interface[index].name)
    {
        set_error_reason("CAN interface name is not set.");
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
            set_error_reason("No CAN interface found at specified index.");
            return -1;
    }
}

CANVENIENT_API int can_send(int index, const struct can_frame* frame)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        set_error_reason("Channel index is out-of-range.");
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
            set_error_reason("No CAN interface found at specified index.");
            return -1;
    }
}

CANVENIENT_API int can_recv(int index, struct can_frame* frame, u64* timestamp)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        set_error_reason("Channel index is out-of-range.");
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
            set_error_reason("No CAN interface found at specified index.");
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
