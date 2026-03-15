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
#include <vcisdk.h>
#include <PCANBasic.h>

#elif defined __linux__
#include <dirent.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#endif

CANVENIENT_API int can_find_interfaces(struct can_iface* iface[], int* count)
{
#ifdef _WIN32
    u32 ch_count = 0;
    TPCANChannelInformation* pcan_ch_info;
    TPCANStatus pcan_status;

    /* PEAK-System. */
    pcan_status = CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS_COUNT, &ch_count, sizeof(u32));
    if (PCAN_ERROR_OK != pcan_status || 0 == ch_count)
    {
        return -1;
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

    *iface = (struct can_iface*)malloc(sizeof(struct can_iface) * ch_count);
    if (NULL == *iface)
    {
        free(pcan_ch_info);
        return -1;
    }

    for (u32 i = 0; i < ch_count; i++)
    {
        size_t name_len;
        (*iface)[i].id = pcan_ch_info[i].channel_handle;

        name_len = strnlen(pcan_ch_info[i].device_name, sizeof(pcan_ch_info[i].device_name));
        (*iface)[i].name = (char*)malloc(name_len + 1);
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

        /* Set interface properties */
        snprintf((*iface)[i].name, name_len + 1, "%.*s", (int)name_len, pcan_ch_info[i].device_name);

        (*iface)[i].id = pcan_ch_info[i].device_id;
        (*iface)[i].vendor = CAN_VENDOR_PEAK;
        (*iface)[i].opened = 0;
        (*iface)[i].baudrate = CAN_BAUD_1M;
    }

    /* Ixxat. */
    /* Tbd. */

    *count = (int)ch_count;
    free(pcan_ch_info);
    return 0;

#elif defined __linux__

    DIR* dir;
    struct dirent* entry;
    int iface_count = 0;
    int capacity = 16;
    struct can_iface* temp_iface;

    /* Open /sys/class/net to enumerate network interfaces */
    dir = opendir("/sys/class/net");
    if (NULL == dir)
    {
        return -1;
    }

    /* Allocate initial array */
    *iface = (struct can_iface*)malloc(sizeof(struct can_iface) * capacity);
    if (NULL == *iface)
    {
        closedir(dir);
        return -1;
    }

    /* Scan through network interfaces */
    while ((entry = readdir(dir)) != NULL)
    {
        char path[512];
        FILE* type_file;
        int if_type;
        size_t name_len;

        /* Skip . and .. */
        if (entry->d_name[0] == '.')
        {
            continue;
        }

        /* Check if this is a CAN interface by reading the type */
        snprintf(path, sizeof(path), "/sys/class/net/%s/type", entry->d_name);
        type_file = fopen(path, "r");
        if (NULL == type_file)
        {
            continue;
        }

        if (fscanf(type_file, "%d", &if_type) != 1)
        {
            fclose(type_file);
            continue;
        }
        fclose(type_file);

        /* ARPHRD_CAN = 280 (CAN interface) */
        if (if_type != 280)
        {
            continue;
        }

        /* Expand array if needed */
        if (iface_count >= capacity)
        {
            capacity *= 2;
            temp_iface = (struct can_iface*)realloc(*iface, sizeof(struct can_iface) * capacity);
            if (NULL == temp_iface)
            {
                for (int i = 0; i < iface_count; i++)
                {
                    free((*iface)[i].name);
                }
                free(*iface);
                closedir(dir);
                return -1;
            }
            *iface = temp_iface;
        }

        /* Allocate and copy interface name */
        name_len = strnlen(entry->d_name, 256);
        (*iface)[iface_count].name = (char*)malloc(name_len + 1);
        if (NULL == (*iface)[iface_count].name)
        {
            for (int i = 0; i < iface_count; i++)
            {
                free((*iface)[i].name);
            }
            free(*iface);
            closedir(dir);
            return -1;
        }

        /* Set interface properties */
        snprintf((*iface)[iface_count].name, name_len + 1, "%.*s", (int)name_len, entry->d_name);

        (*iface)[iface_count].name[name_len] = '\0';
        (*iface)[iface_count].id = if_nametoindex(entry->d_name);
        (*iface)[iface_count].vendor = CAN_VENDOR_SOCKETCAN; /* Generic SocketCAN */
        (*iface)[iface_count].opened = 0;
        (*iface)[iface_count].baudrate = CAN_BAUD_1M;

        iface_count++;
    }

    closedir(dir);

    *count = iface_count;
    return (iface_count > 0) ? 0 : -1;
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
    if (NULL == iface)
    {
        return -1;
    }

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
        default:
            return -1;
    }

#elif defined __linux__

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
        {
            break;
        }
        default:
            break;
    }

    iface->opened = 0;

#elif defined __linux__

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
