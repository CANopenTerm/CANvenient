/** @file CANvenient.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on Windows.
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
    void* internal;
};

static struct can_iface can_interface[CAN_MAX_INTERFACES] = {0};

static int find_free_interface_slot(u32* index);

#ifdef _WIN32
#include <windows.h>
#include <initguid.h>
#include <vcisdk.h>
#include <PCANBasic.h>

typedef struct ixxat_ctx
{
    VCIID device_id;
    u32 bus_no;
    ICanChannel* pChannel;
    IFifoReader* pReader;
    IFifoWriter* pWriter;

} ixxat_ctx_t;

static void ixxat_baudrate_to_btr(enum can_baudrate baud, u8* bt0, u8* bt1);

#elif defined __linux__
#include <dirent.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#endif

CANVENIENT_API int can_find_interfaces(void)
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

        can_interface[i].id = pcan_ch_info[i].channel_handle;

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

        can_interface[i].id = pcan_ch_info[i].device_id;
        can_interface[i].vendor = CAN_VENDOR_PEAK;
        can_interface[i].opened = 0;
        can_interface[i].baudrate = CAN_BAUD_1M;
        can_interface[i].internal = NULL;
    }
    free(pcan_ch_info);

    /* Ixxat. */
    IVciDeviceManager* pDevMan = NULL;
    IVciEnumDevice* pEnum = NULL;
    HRESULT hr;

    hr = VciGetDeviceManager(&pDevMan);
    if (SUCCEEDED(hr) && (pDevMan))
    {
        hr = pDevMan->lpVtbl->EnumDevices(pDevMan, &pEnum);
        if (SUCCEEDED(hr) && (pEnum))
        {
            VCIDEVICEINFO devInfo;
            u32 fetched = 0;

            while (S_OK == pEnum->lpVtbl->Next(pEnum, 1, &devInfo, &fetched) && fetched > 0)
            {
                IVciDevice* pDevice = NULL;
                IBalObject* pBal = NULL;

                hr = pDevMan->lpVtbl->OpenDevice(pDevMan, &devInfo.VciObjectId, &pDevice);
                if (SUCCEEDED(hr) && (pDevice))
                {
                    hr = pDevice->lpVtbl->OpenComponent(pDevice, &CLSID_VCIBAL, &IID_IBalObject, (PVOID*)&pBal);
                    if (SUCCEEDED(hr) && (pBal))
                    {
                        BALFEATURES features;

                        hr = pBal->lpVtbl->GetFeatures(pBal, &features);
                        if (SUCCEEDED(hr))
                        {
                            for (u32 bus = 0; bus < features.BusSocketCount; bus++)
                            {
                                if (features.BusSocketType[bus])
                                {
                                    ixxat_ctx_t* ctx;
                                    char socket_name[256];
                                    size_t name_len;
                                    u32 free_index;

                                    if (0 != find_free_interface_slot(&free_index))
                                    {
                                        /* No free slot available: skip. */
                                        continue;
                                    }

                                    snprintf(socket_name, sizeof(socket_name), "%s CAN%u", devInfo.Description, bus);
                                    name_len = strnlen(socket_name, sizeof(socket_name));

                                    can_interface[free_index].name = (char*)malloc(name_len + 1);
                                    if (NULL == can_interface[free_index].name)
                                    {
                                        break;
                                    }

                                    ctx = (ixxat_ctx_t*)malloc(sizeof(ixxat_ctx_t));
                                    if (NULL == ctx)
                                    {
                                        free(can_interface[free_index].name);
                                        can_interface[free_index].name = NULL;
                                        break;
                                    }
                                    ctx->device_id = devInfo.VciObjectId;
                                    ctx->bus_no = bus;
                                    ctx->pChannel = NULL;
                                    ctx->pReader = NULL;
                                    ctx->pWriter = NULL;

                                    snprintf(can_interface[ch_count].name, name_len + 1, "%s", socket_name);
                                    can_interface[ch_count].id = (u32)bus;
                                    can_interface[ch_count].vendor = CAN_VENDOR_IXXAT;
                                    can_interface[ch_count].opened = 0;
                                    can_interface[ch_count].baudrate = CAN_BAUD_1M;
                                    can_interface[ch_count].internal = ctx;

                                    ch_count++;
                                }
                            }
                        }
                        pBal->lpVtbl->Release(pBal);
                    }
                    pDevice->lpVtbl->Release(pDevice);
                }
            }
            pEnum->lpVtbl->Release(pEnum);
        }
        pDevMan->lpVtbl->Release(pDevMan);
    }

    return 0;

#elif defined __linux__

    DIR* dir;
    struct dirent* entry;

    /* Open /sys/class/net to enumerate network interfaces */
    dir = opendir("/sys/class/net");
    if (NULL == dir)
    {
        return -1;
    }

    /* Scan through network interfaces */
    while ((entry = readdir(dir)))
    {
        char path[512];
        FILE* type_file;
        int if_type;
        size_t name_len;
        u32 free_index;

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

        /* Find a free slot in the interface array */
        if (0 != find_free_interface_slot(&free_index))
        {
            /* No free slot available: skip. */
            continue;
        }

        /* Allocate and copy interface name */
        name_len = strnlen(entry->d_name, 256);
        can_interface[free_index].name = (char*)malloc(name_len + 1);
        if (NULL == can_interface[free_index].name)
        {
            closedir(dir);
            return -1;
        }

        /* Set interface properties */
        snprintf(can_interface[free_index].name, name_len + 1, "%.*s", (int)name_len, entry->d_name);

        can_interface[free_index].id = if_nametoindex(entry->d_name);
        can_interface[free_index].vendor = CAN_VENDOR_SOCKETCAN;
        can_interface[free_index].opened = 0;
        can_interface[free_index].baudrate = CAN_BAUD_1M;
        can_interface[free_index].internal = malloc(sizeof(int)); /* CAN socket. */
    }

    closedir(dir);

    return 0;
#endif
}

CANVENIENT_API void can_free_interfaces(void)
{
    for (int i = 0; i < CAN_MAX_INTERFACES; i++)
    {
        can_close(i);
    }
#if defined __linux__

#endif
}

CANVENIENT_API int can_open(int index)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return -1;
    }

#ifdef _WIN32
    switch (can_interface[index].vendor)
    {
        case CAN_VENDOR_PEAK:
        {
            if (PCAN_ERROR_OK == CAN_Initialize((WORD)can_interface[index].id, can_interface[index].baudrate, 0, 0, 0))
            {
                can_interface[index].opened = 1;
            }
            break;
        }
        case CAN_VENDOR_IXXAT:
        {
            ixxat_ctx_t* ctx;
            IVciDeviceManager* pDevMan = NULL;
            IVciDevice* pDevice = NULL;
            IBalObject* pBal = NULL;
            ICanControl* pControl = NULL;
            ICanSocket* pSocket = NULL;
            ICanChannel* pChannel = NULL;
            CANINITLINE initLine;
            HRESULT hr;
            u8 bt0;
            u8 bt1;

            ctx = (ixxat_ctx_t*)can_interface[index].internal;
            if (NULL == ctx)
            {
                return -1;
            }

            hr = VciGetDeviceManager(&pDevMan);
            if (FAILED(hr) || NULL == pDevMan)
            {
                return -1;
            }

            hr = pDevMan->lpVtbl->OpenDevice(pDevMan, &ctx->device_id, &pDevice);
            pDevMan->lpVtbl->Release(pDevMan);
            if (FAILED(hr) || NULL == pDevice)
            {
                return -1;
            }

            hr = pDevice->lpVtbl->OpenComponent(pDevice, &CLSID_VCIBAL, &IID_IBalObject, (PVOID*)&pBal);
            pDevice->lpVtbl->Release(pDevice);
            if (FAILED(hr) || NULL == pBal)
            {
                return -1;
            }

            hr = pBal->lpVtbl->OpenSocket(pBal, ctx->bus_no, &IID_ICanControl, (PVOID*)&pControl);
            if (SUCCEEDED(hr) && NULL != pControl)
            {
                ixxat_baudrate_to_btr(can_interface[index].baudrate, &bt0, &bt1);
                initLine.bOpMode = CAN_OPMODE_STANDARD | CAN_OPMODE_EXTENDED | CAN_OPMODE_ERRFRAME;
                initLine.bReserved = 0;
                initLine.bBtReg0 = bt0;
                initLine.bBtReg1 = bt1;
                pControl->lpVtbl->InitLine(pControl, &initLine);
                pControl->lpVtbl->StartLine(pControl);
                pControl->lpVtbl->Release(pControl);
            }

            hr = pBal->lpVtbl->OpenSocket(pBal, ctx->bus_no, &IID_ICanSocket, (PVOID*)&pSocket);
            pBal->lpVtbl->Release(pBal);
            if (FAILED(hr) || NULL == pSocket)
            {
                return -1;
            }

            hr = pSocket->lpVtbl->CreateChannel(pSocket, FALSE, &pChannel);
            pSocket->lpVtbl->Release(pSocket);
            if (FAILED(hr) || NULL == pChannel)
            {
                return -1;
            }

            hr = pChannel->lpVtbl->Initialize(pChannel, 1024, 128);
            if (FAILED(hr))
            {
                pChannel->lpVtbl->Release(pChannel);
                return -1;
            }

            hr = pChannel->lpVtbl->Activate(pChannel);
            if (FAILED(hr))
            {
                pChannel->lpVtbl->Release(pChannel);
                return -1;
            }

            pChannel->lpVtbl->GetReader(pChannel, &ctx->pReader);
            pChannel->lpVtbl->GetWriter(pChannel, &ctx->pWriter);
            ctx->pChannel = pChannel;
            can_interface[index].opened = 1;
            break;
        }
        default:
            return -1;
    }

#elif defined __linux__

    if (can_interface[index].name)
    {
        can_interface[index].opened = 1;
        // (int)can_interface[index].internal = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    }
    else
    {
        return -1;
    }

#endif

    return 0;
}

CANVENIENT_API int can_open_fd(int index)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return -1;
    }

    return 0;
}

CANVENIENT_API void can_close(int index)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return;
    }

#ifdef _WIN32
    switch (can_interface[index].vendor)
    {
        case CAN_VENDOR_PEAK:
        {
            CAN_Uninitialize((WORD)can_interface[index].id);
            break;
        }
        case CAN_VENDOR_IXXAT:
        {
            ixxat_ctx_t* ctx = (ixxat_ctx_t*)can_interface[index].internal;
            if (ctx)
            {
                if (ctx->pWriter)
                {
                    ctx->pWriter->lpVtbl->Release(ctx->pWriter);
                    ctx->pWriter = NULL;
                }
                if (ctx->pReader)
                {
                    ctx->pReader->lpVtbl->Release(ctx->pReader);
                    ctx->pReader = NULL;
                }
                if (ctx->pChannel)
                {
                    ctx->pChannel->lpVtbl->Deactivate(ctx->pChannel);
                    ctx->pChannel->lpVtbl->Release(ctx->pChannel);
                    ctx->pChannel = NULL;
                }
            }
            break;
        }
        case CAN_VENDOR_NONE:
        default:
            break;
    }

    can_interface[index].vendor = CAN_VENDOR_NONE;
    can_interface[index].opened = 0;

    if (! can_interface[index].name)
    {
        free(can_interface[index].name);
    }
    if (! can_interface[index].internal)
    {
        free(can_interface[index].internal);
        can_interface[index].internal = NULL;
    }

    return;

#endif
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

CANVENIENT_API int can_send(int index, struct can_frame* frame)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return -1;
    }

    (void)frame;
    return 0;
}

CANVENIENT_API int can_recv(int index, struct can_frame* frame)
{
    if (index < 0 || index >= CAN_MAX_INTERFACES)
    {
        return -1;
    }

    (void)frame;
    return 0;
}

static int find_free_interface_slot(u32* index)
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

#ifdef _WIN32
static void ixxat_baudrate_to_btr(enum can_baudrate baud, u8* bt0, u8* bt1)
{
    switch (baud)
    {
        case CAN_BAUD_800K:
            *bt0 = CAN_BT0_800KB;
            *bt1 = CAN_BT1_800KB;
            break;
        case CAN_BAUD_500K:
            *bt0 = CAN_BT0_500KB;
            *bt1 = CAN_BT1_500KB;
            break;
        case CAN_BAUD_250K:
            *bt0 = CAN_BT0_250KB;
            *bt1 = CAN_BT1_250KB;
            break;
        case CAN_BAUD_125K:
            *bt0 = CAN_BT0_125KB;
            *bt1 = CAN_BT1_125KB;
            break;
        case CAN_BAUD_100K:
        case CAN_BAUD_95K:
            *bt0 = CAN_BT0_100KB;
            *bt1 = CAN_BT1_100KB;
            break;
        case CAN_BAUD_83K:
            *bt0 = 0x0B;
            *bt1 = 0x14;
            break;
        case CAN_BAUD_50K:
        case CAN_BAUD_47K:
            *bt0 = CAN_BT0_50KB;
            *bt1 = CAN_BT1_50KB;
            break;
        case CAN_BAUD_33K:
        case CAN_BAUD_20K:
            *bt0 = CAN_BT0_20KB;
            *bt1 = CAN_BT1_20KB;
            break;
        case CAN_BAUD_10K:
            *bt0 = CAN_BT0_10KB;
            *bt1 = CAN_BT1_10KB;
            break;
        case CAN_BAUD_5K:
            *bt0 = CAN_BT0_5KB;
            *bt1 = CAN_BT1_5KB;
            break;
        case CAN_BAUD_1M:
        default:
            *bt0 = CAN_BT0_1000KB;
            *bt1 = CAN_BT1_1000KB;
            break;
    }
}
#endif
