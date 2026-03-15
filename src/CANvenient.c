/** @file CANvenient.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on Windows,
 *  similar to libsocketcan on Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CANvenient.h"

#ifdef _WIN32
#include <windows.h>
#include <PCANBasic.h>

#elif __linux__
#endif

CANVENIENT_API int can_find_interfaces(struct can_iface* iface[], int* count)
{
#ifdef _WIN32
    u32 pcan_ch_count = 0;
    TPCANChannelInformation* pcan_ch_info;
    TPCANStatus pcan_status;

    pcan_status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS_COUNT, &pcan_ch_count, sizeof(u32));
    if (PCAN_ERROR_OK != pcan_status || 0 == pcan_ch_count)
    {
        return -1;
    }

    pcan_ch_info = (TPCANChannelInformation*)malloc(sizeof(TPCANChannelInformation) * pcan_ch_count);
    if (NULL == pcan_ch_info)
    {
        return -1;
    }

    pcan_status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS, pcan_ch_info, sizeof(TPCANChannelInformation) * pcan_ch_count);
    if (PCAN_ERROR_OK != pcan_status)
    {
        free(pcan_ch_info);
        pcan_ch_info = NULL;
        return -1;
    }

    *iface = (struct can_iface*)malloc(sizeof(struct can_iface) * pcan_ch_count);
    if (NULL == *iface)
    {
        free(pcan_ch_info);
        return -1;
    }

    for (u32 i = 0; i < pcan_ch_count; i++)
    {
        (*iface)[i].id = pcan_ch_info[i].channel_handle;
        (*iface)[i].name = (char*)malloc(strlen(pcan_ch_info[i].device_name) + 1);
        if (NULL == (*iface)[i].name)
        {
            for (u32 j = 0; j < i; j++)
            {
                free((*iface)[j].name);
            }
            free(*iface);
            free(pcan_ch_info);
            return -1;
        }

        snprintf((*iface)[i].name, strlen(pcan_ch_info[i].device_name) + 1, "%s", pcan_ch_info[i].device_name);

        (*iface)[i].vendor = CAN_VENDOR_PEAK;
        (*iface)[i].id = pcan_ch_info[i].device_id;
    }

    *count = (int)pcan_ch_count;
    free(pcan_ch_info);
    return 0;

#else
    (void)iface;
    (void)count;
    return -1;
#endif
}

CANVENIENT_API void can_free_interfaces(struct can_iface* iface[], int count)
{
    for (int i = 0; i < count; i++)
    {
        can_close(iface[i]);
        if (iface[i] != NULL)
        {
            free(iface[i]->name);
        }
    }
    free(*iface);
}

CANVENIENT_API int can_open(struct can_iface* iface)
{
    if (NULL == iface->name)
    {
        return -1;
    }

#ifdef _WIN32
    switch (iface->vendor)
    {
        case CAN_VENDOR_PEAK:
        {
            if (PCAN_ERROR_OK == CAN_Initialize((WORD)iface->id, iface->baudrate, 0, 0, 0))
            {
                iface->opened = 1;
            }
            break;
        }
        case CAN_VENDOR_IXXAT:
        case CAN_VENDOR_KVASER:
        default:
            return -1;
    }
#endif

    return 0;
}

CANVENIENT_API int can_open_fd(struct can_iface* iface)
{
    (void)iface;
    return 0;
}

CANVENIENT_API void can_close(struct can_iface* iface)
{
#ifdef _WIN32
    switch (iface->vendor)
    {
        case CAN_VENDOR_PEAK:
        {
            CAN_Uninitialize((WORD)iface->id);
            break;
        }
        case CAN_VENDOR_IXXAT:
        case CAN_VENDOR_KVASER:
        default:
            break;
    }

    iface->opened = 0;
#endif
}

CANVENIENT_API int can_send(struct can_iface* iface, struct can_frame* frame)
{
    (void)iface;
    (void)frame;
    return 0;
}

CANVENIENT_API int can_recv(struct can_iface* iface, struct can_frame* frame)
{
    (void)iface;
    (void)frame;
    return 0;
}
