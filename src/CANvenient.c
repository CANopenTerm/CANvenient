/** @file CANvenient.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on Windows.
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

static int* can_socket;
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
        (*iface)[i].internal = NULL;
    }

    /* Ixxat. */
    IVciDeviceManager* pDevMan = NULL;
    IVciEnumDevice* pEnum = NULL;
    HRESULT hr;

    hr = VciGetDeviceManager(&pDevMan);
    if (SUCCEEDED(hr) && (NULL != pDevMan))
    {
        hr = pDevMan->lpVtbl->EnumDevices(pDevMan, &pEnum);
        if (SUCCEEDED(hr) && (NULL != pEnum))
        {
            VCIDEVICEINFO devInfo;
            u32 fetched = 0;

            while (S_OK == pEnum->lpVtbl->Next(pEnum, 1, &devInfo, &fetched) && fetched > 0)
            {
                IVciDevice* pDevice = NULL;
                IBalObject* pBal = NULL;

                hr = pDevMan->lpVtbl->OpenDevice(pDevMan, &devInfo.VciObjectId, &pDevice);
                if (SUCCEEDED(hr) && (NULL != pDevice))
                {
                    hr = pDevice->lpVtbl->OpenComponent(pDevice, &CLSID_VCIBAL, &IID_IBalObject, (PVOID*)&pBal);
                    if (SUCCEEDED(hr) && (NULL != pBal))
                    {
                        BALFEATURES features;

                        hr = pBal->lpVtbl->GetFeatures(pBal, &features);
                        if (SUCCEEDED(hr))
                        {
                            for (u32 bus = 0; bus < features.BusSocketCount; bus++)
                            {
                                if (features.BusSocketType[bus])
                                {
                                    struct can_iface* new_iface;
                                    ixxat_ctx_t* ctx;
                                    char socket_name[256];
                                    size_t name_len;

                                    new_iface = (struct can_iface*)realloc(*iface, sizeof(struct can_iface) * (ch_count + 1));
                                    if (NULL == new_iface)
                                    {
                                        break;
                                    }
                                    *iface = new_iface;

                                    snprintf(socket_name, sizeof(socket_name), "%s CAN%u", devInfo.Description, bus);
                                    name_len = strnlen(socket_name, sizeof(socket_name));

                                    (*iface)[ch_count].name = (char*)malloc(name_len + 1);
                                    if (NULL == (*iface)[ch_count].name)
                                    {
                                        break;
                                    }

                                    ctx = (ixxat_ctx_t*)malloc(sizeof(ixxat_ctx_t));
                                    if (NULL == ctx)
                                    {
                                        free((*iface)[ch_count].name);
                                        (*iface)[ch_count].name = NULL;
                                        break;
                                    }
                                    ctx->device_id = devInfo.VciObjectId;
                                    ctx->bus_no = bus;
                                    ctx->pChannel = NULL;
                                    ctx->pReader = NULL;
                                    ctx->pWriter = NULL;

                                    snprintf((*iface)[ch_count].name, name_len + 1, "%s", socket_name);
                                    (*iface)[ch_count].id = (u32)bus;
                                    (*iface)[ch_count].vendor = CAN_VENDOR_IXXAT;
                                    (*iface)[ch_count].opened = 0;
                                    (*iface)[ch_count].baudrate = CAN_BAUD_1M;
                                    (*iface)[ch_count].internal = ctx;

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
    can_socket = (int*)malloc(sizeof(int) * capacity);
    if (NULL == can_socket)
    {
        free(*iface);
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
                free(can_socket);
                free(*iface);
                closedir(dir);
                return -1;
            }
            can_socket = (int*)realloc(can_socket, sizeof(int) * capacity);
            if (NULL == can_socket)
            {
                for (int i = 0; i < iface_count; i++)
                {
                    free((*iface)[i].name);
                }
                free(can_socket);
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
            free(can_socket);
            free(*iface);
            closedir(dir);
            return -1;
        }

        /* Set interface properties */
        snprintf((*iface)[iface_count].name, name_len + 1, "%.*s", (int)name_len, entry->d_name);

        (*iface)[iface_count].name[name_len] = '\0';
        (*iface)[iface_count].id = if_nametoindex(entry->d_name);
        (*iface)[iface_count].vendor = CAN_VENDOR_SOCKETCAN;
        (*iface)[iface_count].opened = 0;
        (*iface)[iface_count].baudrate = CAN_BAUD_1M;
        (*iface)[iface_count].internal = NULL;

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
        can_close(&(*iface)[i]);
        if ((*iface)[i].name != NULL)
        {
            free((*iface)[i].name);
        }
        if ((*iface)[i].internal != NULL)
        {
            free((*iface)[i].internal);
            (*iface)[i].internal = NULL;
        }
    }
    free(*iface);
#if defined __linux__
    free(can_socket);
#endif
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
            UINT8 bt0;
            UINT8 bt1;

            ctx = (ixxat_ctx_t*)iface->internal;
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
                ixxat_baudrate_to_btr(iface->baudrate, &bt0, &bt1);
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
            iface->opened = 1;
            break;
        }
        default:
            return -1;
    }

#elif defined __linux__

    iface->opened = 1;
    /* Tbd. */

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
    if (NULL == iface)
    {
        return;
    }

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
            ixxat_ctx_t* ctx = (ixxat_ctx_t*)iface->internal;
            if (NULL != ctx)
            {
                if (NULL != ctx->pWriter)
                {
                    ctx->pWriter->lpVtbl->Release(ctx->pWriter);
                    ctx->pWriter = NULL;
                }
                if (NULL != ctx->pReader)
                {
                    ctx->pReader->lpVtbl->Release(ctx->pReader);
                    ctx->pReader = NULL;
                }
                if (NULL != ctx->pChannel)
                {
                    ctx->pChannel->lpVtbl->Deactivate(ctx->pChannel);
                    ctx->pChannel->lpVtbl->Release(ctx->pChannel);
                    ctx->pChannel = NULL;
                }
            }
            break;
        }
        default:
            break;
    }

    iface->opened = 0;

#elif defined __linux__

    return;

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
