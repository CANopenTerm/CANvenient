/** @file CANvenient_Ixxat.c
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

#include "CANvenient.h"
#include "CANvenient_internal.h"

#ifdef _WIN32
#include <windows.h>
#include <initguid.h>
#include <vcisdk.h>

typedef struct ixxat_ctx
{
    VCIID device_id;
    u32 bus_no;
    ICanObject* pCanObject;
    IFifoReader* pReader;
    IFifoWriter* pWriter;

} ixxat_ctx_t;

static void ixxat_baudrate_to_btr(enum can_baudrate baud, u8* bt0, u8* bt1);

#endif

int ixxat_find_interfaces(void)
{
#ifdef _WIN32

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
                                if (VCI_BUS_TYPE(features.BusSocketType[bus]) == VCI_BUS_CAN)
                                {
                                    ixxat_ctx_t* ctx;
                                    char socket_name[256];
                                    size_t name_len;
                                    u32 free_index;
                                    u32 k;
                                    int already_registered = 0;

                                    snprintf(socket_name, sizeof(socket_name), "%s CAN%u", devInfo.Description, bus);

                                    for (k = 0; k < CAN_MAX_INTERFACES; k++)
                                    {
                                        if (can_interface[k].name && strcmp(can_interface[k].name, socket_name) == 0)
                                        {
                                            already_registered = 1;
                                            break;
                                        }
                                    }
                                    if (already_registered)
                                    {
                                        continue;
                                    }

                                    if (0 != find_free_interface_slot(&free_index))
                                    {
                                        /* No free slot available: stop. */
                                        break;
                                    }

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
                                    ctx->pCanObject = NULL;
                                    ctx->pReader = NULL;
                                    ctx->pWriter = NULL;

                                    snprintf(can_interface[free_index].name, name_len + 1, "%s", socket_name);
                                    can_interface[free_index].vendor = CAN_VENDOR_IXXAT;
                                    can_interface[free_index].opened = 0;
                                    can_interface[free_index].baudrate = CAN_BAUD_1M;
                                    can_interface[free_index].internal = ctx;
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

#endif

    return 0;
}

int ixxat_open(int index)
{
#ifdef _WIN32

    ixxat_ctx_t* ctx;
    IVciDeviceManager* pDevMan = NULL;
    IVciDevice* pDevice = NULL;
    IBalObject* pBal = NULL;
    ICanObject* pCanObj = NULL;
    CANINITLINE initLine;
    HRESULT hr;
    u8 bt0;
    u8 bt1;

    ctx = (ixxat_ctx_t*)can_interface[index].internal;
    if (NULL == ctx)
    {
        set_error_reason("Internal error: context is NULL.");
        return -1;
    }

    hr = VciGetDeviceManager(&pDevMan);
    if (FAILED(hr) || NULL == pDevMan)
    {
        set_error_reason("Failed to get device manager.");
        return -1;
    }

    hr = pDevMan->lpVtbl->OpenDevice(pDevMan, &ctx->device_id, &pDevice);
    pDevMan->lpVtbl->Release(pDevMan);
    if (FAILED(hr) || NULL == pDevice)
    {
        set_error_reason("Failed to open device.");
        return -1;
    }

    hr = pDevice->lpVtbl->OpenComponent(pDevice, &CLSID_VCIBAL, &IID_IBalObject, (PVOID*)&pBal);
    pDevice->lpVtbl->Release(pDevice);
    if (FAILED(hr) || NULL == pBal)
    {
        set_error_reason("Failed to open BAL component.");
        return -1;
    }

    hr = pBal->lpVtbl->OpenSocket(pBal, ctx->bus_no, &IID_ICanObject, (PVOID*)&pCanObj);
    pBal->lpVtbl->Release(pBal);
    if (FAILED(hr) || NULL == pCanObj)
    {
        set_error_reason("Failed to open CAN object.");
        return -1;
    }

    ixxat_baudrate_to_btr(can_interface[index].baudrate, &bt0, &bt1);
    initLine.bOpMode   = CAN_OPMODE_STANDARD | CAN_OPMODE_EXTENDED | CAN_OPMODE_ERRFRAME;
    initLine.bReserved = 0;
    initLine.bBtReg0   = bt0;
    initLine.bBtReg1   = bt1;

    pCanObj->lpVtbl->ResetLine(pCanObj);
    hr = pCanObj->lpVtbl->InitLine(pCanObj, &initLine, 1024, 128);
    if (FAILED(hr))
    {
        pCanObj->lpVtbl->Release(pCanObj);
        set_error_reason("Failed to initialize CAN line.");
        return -1;
    }

    pCanObj->lpVtbl->SetAccFilter(pCanObj, CAN_FILTER_STD, CAN_ACC_CODE_ALL, CAN_ACC_MASK_ALL);
    pCanObj->lpVtbl->SetAccFilter(pCanObj, CAN_FILTER_EXT, CAN_ACC_CODE_ALL, CAN_ACC_MASK_ALL);

    hr = pCanObj->lpVtbl->StartLine(pCanObj);
    if (FAILED(hr))
    {
        pCanObj->lpVtbl->Release(pCanObj);
        set_error_reason("Failed to start CAN line.");
        return -1;
    }

    pCanObj->lpVtbl->GetReceiveFifo(pCanObj, &ctx->pReader);
    pCanObj->lpVtbl->GetTransmitFifo(pCanObj, &ctx->pWriter);
    ctx->pCanObject = pCanObj;
    can_interface[index].opened = 1;

    return 0;

#else
    set_error_reason("Ixxat driver is only supported on Windows.");
    (void)index;
    return -1;
#endif
}

void ixxat_close(int index)
{
#ifdef _WIN32

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
        if (ctx->pCanObject)
        {
            ctx->pCanObject->lpVtbl->StopLine(ctx->pCanObject);
            ctx->pCanObject->lpVtbl->Release(ctx->pCanObject);
            ctx->pCanObject = NULL;
        }
    }
    can_interface[index].opened = 0;

#else
    set_error_reason("Ixxat driver is only supported on Windows.");
    (void)index;
#endif
}

int ixxat_update(int index)
{
#ifdef _WIN32

    ixxat_ctx_t* ctx;
    IVciDeviceManager* pDevMan = NULL;
    IVciDevice* pDevice = NULL;
    HRESULT hr;

    ctx = (ixxat_ctx_t*)can_interface[index].internal;
    if (NULL == ctx)
    {
        return -1;
    }

    hr = VciGetDeviceManager(&pDevMan);
    if (FAILED(hr) || NULL == pDevMan)
    {
        can_release(index);
        return -1;
    }

    hr = pDevMan->lpVtbl->OpenDevice(pDevMan, &ctx->device_id, &pDevice);
    pDevMan->lpVtbl->Release(pDevMan);
    if (FAILED(hr) || NULL == pDevice)
    {
        can_release(index);
        return -1;
    }

    pDevice->lpVtbl->Release(pDevice);
    return 0;

#else
    set_error_reason("Ixxat driver is only supported on Windows.");
    (void)index;
    return -1;
#endif
}

int ixxat_set_baudrate(int index, enum can_baudrate baud)
{
#ifdef _WIN32

    ixxat_ctx_t* ctx = (ixxat_ctx_t*)can_interface[index].internal;
    IVciDeviceManager* pDevMan = NULL;
    IVciDevice* pDevice = NULL;
    IBalObject* pBal = NULL;
    ICanObject* pCanObj = NULL;
    CANINITLINE initLine;
    HRESULT hr;
    u8 bt0;
    u8 bt1;

    if (NULL == ctx)
    {
        set_error_reason("Internal error: context is NULL.");
        return -1;
    }

    /* Release existing resources. */
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
    if (ctx->pCanObject)
    {
        ctx->pCanObject->lpVtbl->StopLine(ctx->pCanObject);
        ctx->pCanObject->lpVtbl->Release(ctx->pCanObject);
        ctx->pCanObject = NULL;
    }

    hr = VciGetDeviceManager(&pDevMan);
    if (FAILED(hr) || NULL == pDevMan)
    {
        set_error_reason("Failed to get device manager.");
        return -1;
    }

    hr = pDevMan->lpVtbl->OpenDevice(pDevMan, &ctx->device_id, &pDevice);
    pDevMan->lpVtbl->Release(pDevMan);
    if (FAILED(hr) || NULL == pDevice)
    {
        set_error_reason("Failed to open device.");
        return -1;
    }

    hr = pDevice->lpVtbl->OpenComponent(pDevice, &CLSID_VCIBAL, &IID_IBalObject, (PVOID*)&pBal);
    pDevice->lpVtbl->Release(pDevice);
    if (FAILED(hr) || NULL == pBal)
    {
        set_error_reason("Failed to open BAL component.");
        return -1;
    }

    hr = pBal->lpVtbl->OpenSocket(pBal, ctx->bus_no, &IID_ICanObject, (PVOID*)&pCanObj);
    pBal->lpVtbl->Release(pBal);
    if (FAILED(hr) || NULL == pCanObj)
    {
        set_error_reason("Failed to open CAN object.");
        return -1;
    }

    ixxat_baudrate_to_btr(baud, &bt0, &bt1);
    initLine.bOpMode   = CAN_OPMODE_STANDARD | CAN_OPMODE_EXTENDED | CAN_OPMODE_ERRFRAME;
    initLine.bReserved = 0;
    initLine.bBtReg0   = bt0;
    initLine.bBtReg1   = bt1;

    pCanObj->lpVtbl->ResetLine(pCanObj);
    hr = pCanObj->lpVtbl->InitLine(pCanObj, &initLine, 1024, 128);
    if (FAILED(hr))
    {
        pCanObj->lpVtbl->Release(pCanObj);
        set_error_reason("Failed to initialize CAN line.");
        return -1;
    }

    pCanObj->lpVtbl->SetAccFilter(pCanObj, CAN_FILTER_STD, CAN_ACC_CODE_ALL, CAN_ACC_MASK_ALL);
    pCanObj->lpVtbl->SetAccFilter(pCanObj, CAN_FILTER_EXT, CAN_ACC_CODE_ALL, CAN_ACC_MASK_ALL);

    hr = pCanObj->lpVtbl->StartLine(pCanObj);
    if (FAILED(hr))
    {
        pCanObj->lpVtbl->Release(pCanObj);
        set_error_reason("Failed to start CAN line.");
        return -1;
    }

    pCanObj->lpVtbl->GetReceiveFifo(pCanObj, &ctx->pReader);
    pCanObj->lpVtbl->GetTransmitFifo(pCanObj, &ctx->pWriter);
    ctx->pCanObject = pCanObj;
    can_interface[index].baudrate = baud;

    return 0;

#else
    set_error_reason("Ixxat driver is only supported on Windows.");
    (void)index;
    (void)baud;
    return -1;
#endif
}

int ixxat_send(int index, const struct can_frame* frame)
{
#ifdef _WIN32

    ixxat_ctx_t* ctx = (ixxat_ctx_t*)can_interface[index].internal;
    CANMSG msg = {0};
    HRESULT hr;
    int i;

    if (NULL == ctx || NULL == ctx->pWriter)
    {
        set_error_reason("Channel not open.");
        return -1;
    }

    msg.dwMsgId = frame->can_id;
    msg.uMsgInfo.Bytes.bType = CAN_MSGTYPE_DATA;
    msg.uMsgInfo.Bytes.bFlags = (frame->can_dlc & CAN_MSGFLAGS_DLC) |
                                 (frame->can_id > 0x7FF ? CAN_MSGFLAGS_EXT : 0);

    for (i = 0; i < frame->can_dlc && i < 8; i += 1)
    {
        msg.abData[i] = frame->data[i];
    }

    hr = ctx->pWriter->lpVtbl->PutDataEntry(ctx->pWriter, &msg);
    if (FAILED(hr))
    {
        set_error_reason("Failed to send CAN message.");
        return -1;
    }

    return 0;

#else
    set_error_reason("Ixxat driver is only supported on Windows.");
    (void)index;
    (void)frame;
    return -1;
#endif
}

int ixxat_recv(int index, struct can_frame* frame, u64* timestamp)
{
#ifdef _WIN32

    ixxat_ctx_t* ctx = (ixxat_ctx_t*)can_interface[index].internal;
    CANMSG msg = {0};
    HRESULT hr;
    int i;

    if (NULL == ctx || NULL == ctx->pReader)
    {
        set_error_reason("Channel not open.");
        return -1;
    }

    do
    {
        hr = ctx->pReader->lpVtbl->GetDataEntry(ctx->pReader, &msg);
        if (S_OK != hr)
        {
            return -1;
        }
    } while (CAN_MSGTYPE_DATA != msg.uMsgInfo.Bytes.bType);

    frame->can_id = msg.dwMsgId;
    frame->can_dlc = msg.uMsgInfo.Bytes.bFlags & CAN_MSGFLAGS_DLC;
    *timestamp = (u64)msg.dwTime;

    for (i = 0; i < frame->can_dlc && i < 8; i += 1)
    {
        frame->data[i] = msg.abData[i];
    }

    return 0;

#else
    set_error_reason("Ixxat driver is only supported on Windows.");
    (void)index;
    (void)frame;
    (void)timestamp;
    return -1;
#endif
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
