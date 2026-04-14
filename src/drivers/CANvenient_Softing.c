/** @file CANvenient_Softing.c
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

#ifdef VENIENT_SOFTING_DRV_DISABLED
  #pragma message("!!!!!! WARNING: Softing driver is disabled. !!!!!!")
#endif

#if defined(_WIN32) && !defined(VENIENT_SOFTING_DRV_DISABLED)
#include <windows.h>
#include <Can_def.h>
#include <CANL2.H>

#define CAN_EFF_FLAG 0x80000000U

typedef struct softing_ctx
{
    CAN_HANDLE hChannel;
    char channel_name[MAXLENCHNAME];

} softing_ctx_t;

static double softing_baudrate_to_kbps(enum can_baudrate baud);
#endif

int softing_find_interfaces(void)
{
#if defined(_WIN32) && !defined(VENIENT_SOFTING_DRV_DISABLED)

    unsigned long buf_size_needed = 0;
    unsigned long num_channels = 0;
    CHDSNAPSHOT* channels = NULL;
    unsigned long i;

    CANL2_get_all_CAN_channels(0, &buf_size_needed, &num_channels, NULL);

    if (0 == num_channels || 0 == buf_size_needed)
    {
        return 0;
    }

    channels = (CHDSNAPSHOT*)malloc(buf_size_needed);
    if (NULL == channels)
    {
        set_error_reason("Memory allocation failed.");
        return -1;
    }

    if (CANL2_OK != CANL2_get_all_CAN_channels(buf_size_needed, &buf_size_needed, &num_channels, channels))
    {
        set_error_reason("Failed to enumerate Softing CAN channels.");
        free(channels);
        return -1;
    }

    for (i = 0; i < num_channels; i++)
    {
        softing_ctx_t* ctx;
        size_t name_len;
        u32 free_index;
        u32 k;
        int already_registered = 0;

        for (k = 0; k < CAN_MAX_INTERFACES; k++)
        {
            if (can_interface[k].name && strcmp(can_interface[k].name, channels[i].ChannelName) == 0)
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
            break;
        }

        ctx = (softing_ctx_t*)malloc(sizeof(softing_ctx_t));
        if (NULL == ctx)
        {
            set_error_reason("Memory allocation failed.");
            free(channels);
            return -1;
        }

        ctx->hChannel = 0;
        strncpy_s(ctx->channel_name, MAXLENCHNAME, channels[i].ChannelName, _TRUNCATE);
        ctx->channel_name[MAXLENCHNAME - 1] = '\0';

        name_len = strnlen(channels[i].ChannelName, MAXLENCHNAME);
        can_interface[free_index].name = (char*)malloc(name_len + 1);
        if (NULL == can_interface[free_index].name)
        {
            set_error_reason("Memory allocation failed.");
            free(ctx);
            free(channels);
            return -1;
        }

        snprintf(can_interface[free_index].name, name_len + 1, "%.*s", (int)name_len, channels[i].ChannelName);
        can_interface[free_index].internal = ctx;
        can_interface[free_index].vendor = CAN_VENDOR_SOFTING;
        can_interface[free_index].opened = 0;
        can_interface[free_index].baudrate = CAN_BAUD_1M;
    }

    free(channels);

#endif

    return 0;
}

int softing_open(int index)
{
#if defined(_WIN32) && !defined(VENIENT_SOFTING_DRV_DISABLED)

    softing_ctx_t* ctx = (softing_ctx_t*)can_interface[index].internal;
    L2CONFIG cfg;
    int ret;

    if (NULL == ctx)
    {
        set_error_reason("Internal error: context is NULL.");
        return -1;
    }

    ret = INIL2_initialize_channel(&ctx->hChannel, ctx->channel_name);
    if (ret)
    {
        switch (ret)
        {
            case -1000:
                set_error_reason("Invalid channel handle.");
                break;
            case -1002:
                set_error_reason("Too many channels opened.");
                break;
            case -1003:
                set_error_reason("Version conflict: incompatible driver or DLL.");
                break;
            case -1004:
                set_error_reason("Firmware download error (may be a DPRAM access error).");
                break;
            case -1005:
                set_error_reason("CAN USB DLL (canusbm.dll) not found or could not be loaded.");
                break;
            default:
                set_error_reason("Unknown error while initializing Softing CAN channel.");
                break;
        }
        return -1;
    }

    cfg.fBaudrate = softing_baudrate_to_kbps(can_interface[index].baudrate);
    cfg.s32Prescaler = GET_FROM_SCIM;
    cfg.s32Tseg1 = GET_FROM_SCIM;
    cfg.s32Tseg2 = GET_FROM_SCIM;
    cfg.s32Sjw = GET_FROM_SCIM;
    cfg.s32Sam = GET_FROM_SCIM;
    cfg.s32AccCodeStd = GET_FROM_SCIM;
    cfg.s32AccMaskStd = GET_FROM_SCIM;
    cfg.s32AccCodeXtd = GET_FROM_SCIM;
    cfg.s32AccMaskXtd = GET_FROM_SCIM;
    cfg.s32OutputCtrl = GET_FROM_SCIM;
    cfg.bEnableAck = GET_FROM_SCIM;
    cfg.bEnableErrorframe = GET_FROM_SCIM;
    cfg.hEvent = NULL;

    ret = CANL2_initialize_fifo_mode(ctx->hChannel, &cfg);
    if (CANL2_OK != ret)
    {
        char err_msg[128];
        snprintf(err_msg, sizeof(err_msg), "Failed to configure Softing CAN channel (code: %d).", ret);
        set_error_reason(err_msg);
        INIL2_close_channel(ctx->hChannel);
        ctx->hChannel = 0;
        return -1;
    }

    ret = CANL2_start_chip(ctx->hChannel);
    if (CANL2_OK != ret)
    {
        set_error_reason("Failed to start Softing CAN chip.");
        INIL2_close_channel(ctx->hChannel);
        ctx->hChannel = 0;
        return -1;
    }

    can_interface[index].opened = 1;
    return 0;

#else
  #ifdef VENIENT_SOFTING_DRV_DISABLED
    set_error_reason("Softing driver is disabled.");
  #else
    set_error_reason("Softing driver is only supported on Windows.");
  #endif  
    (void)index;
    return -1;
#endif
}

void softing_close(int index)
{
#if defined(_WIN32) && !defined(VENIENT_SOFTING_DRV_DISABLED)

    softing_ctx_t* ctx = (softing_ctx_t*)can_interface[index].internal;

    if (NULL == ctx)
    {
        return;
    }

    if (ctx->hChannel)
    {
        INIL2_close_channel(ctx->hChannel);
        ctx->hChannel = 0;
    }

    can_interface[index].opened = 0;

#else
  #ifdef VENIENT_SOFTING_DRV_DISABLED
    set_error_reason("Softing driver is disabled.");
  #else
    set_error_reason("Softing driver is only supported on Windows.");
  #endif  
    (void)index;
#endif
}

int softing_update(int index)
{
#if defined(_WIN32) && !defined(VENIENT_SOFTING_DRV_DISABLED)

    const softing_ctx_t* ctx;
    unsigned long buf_size_needed = 0;
    unsigned long num_channels = 0;
    CHDSNAPSHOT* channels = NULL;
    unsigned long i;
    int found = 0;

    ctx = (const softing_ctx_t*)can_interface[index].internal;
    if (NULL == ctx)
    {
        return -1;
    }

    CANL2_get_all_CAN_channels(0, &buf_size_needed, &num_channels, NULL);
    if (0 == num_channels || 0 == buf_size_needed)
    {
        can_release(index);
        return -1;
    }

    channels = (CHDSNAPSHOT*)malloc(buf_size_needed);
    if (NULL == channels)
    {
        return -1;
    }

    if (CANL2_OK != CANL2_get_all_CAN_channels(buf_size_needed, &buf_size_needed, &num_channels, channels))
    {
        free(channels);
        can_release(index);
        return -1;
    }

    for (i = 0; i < num_channels; i++)
    {
        if (0 == strncmp(ctx->channel_name, channels[i].ChannelName, MAXLENCHNAME))
        {
            found = 1;
            break;
        }
    }

    free(channels);

    if (! found)
    {
        can_release(index);
        return -1;
    }

    return 0;

#else
  #ifdef VENIENT_SOFTING_DRV_DISABLED
    set_error_reason("Softing driver is disabled.");
  #else
    set_error_reason("Softing driver is only supported on Windows.");
  #endif  
    (void)index;
    return -1;
#endif
}

int softing_set_baudrate(int index, enum can_baudrate baud)
{
#if defined(_WIN32) && !defined(VENIENT_SOFTING_DRV_DISABLED)

    softing_close(index);
    can_interface[index].baudrate = baud;
    return softing_open(index);

#else
  #ifdef VENIENT_SOFTING_DRV_DISABLED
    set_error_reason("Softing driver is disabled.");
  #else
    set_error_reason("Softing driver is only supported on Windows.");
  #endif  
    (void)index;
    (void)baud;
    return -1;
#endif
}

int softing_send(int index, struct can_frame* frame)
{
#if defined(_WIN32) && !defined(VENIENT_SOFTING_DRV_DISABLED)

    softing_ctx_t* ctx = (softing_ctx_t*)can_interface[index].internal;
    unsigned long ident;
    int xtd;
    int ret;

    if (NULL == ctx || 0 == ctx->hChannel)
    {
        set_error_reason("Channel not open.");
        return -1;
    }

    if (frame->can_id & CAN_EFF_FLAG)
    {
        ident = frame->can_id & ~CAN_EFF_FLAG;
        xtd = 1;
    }
    else
    {
        ident = frame->can_id;
        xtd = 0;
    }

    ret = CANL2_send_data(ctx->hChannel, ident, xtd, (int)frame->can_dlc, frame->data);
    if (CANL2_OK != ret)
    {
        set_error_reason("Failed to send CAN message.");
        return -1;
    }

    return 0;

#else
  #ifdef VENIENT_SOFTING_DRV_DISABLED
    set_error_reason("Softing driver is disabled.");
  #else
    set_error_reason("Softing driver is only supported on Windows.");
  #endif  
    (void)index;
    (void)frame;
    return -1;
#endif
}

int softing_recv(int index, struct can_frame* frame, u64* timestamp)
{
#if defined(_WIN32) && !defined(VENIENT_SOFTING_DRV_DISABLED)

    softing_ctx_t* ctx = (softing_ctx_t*)can_interface[index].internal;
    PARAM_STRUCT param;
    int ret;

    if (NULL == ctx || 0 == ctx->hChannel)
    {
        set_error_reason("Channel not open.");
        return -1;
    }

    memset(&param, 0, sizeof(param));
    ret = CANL2_read_ac(ctx->hChannel, &param);

    if (CANL2_RA_NO_DATA == ret)
    {
        set_error_reason("No message available.");
        return -1;
    }
    else if (ret < 0)
    {
        set_error_reason("Failed to receive CAN message.");
        return -1;
    }

    if (CANL2_RA_DATAFRAME == ret || CANL2_RA_XTD_DATAFRAME == ret)
    {
        frame->can_id = (CANL2_RA_XTD_DATAFRAME == ret) ? (param.Ident | CAN_EFF_FLAG) : param.Ident;
        frame->can_dlc = (u8)param.DataLength;
        *timestamp = (u64)param.Time;

        for (int i = 0; i < param.DataLength && i < 8; i++)
        {
            frame->data[i] = param.RCV_data[i];
        }

        return 0;
    }

    set_error_reason("No message available.");
    return -1;

#else
  #ifdef VENIENT_SOFTING_DRV_DISABLED
    set_error_reason("Softing driver is disabled.");
  #else
    set_error_reason("Softing driver is only supported on Windows.");
  #endif  
    (void)index;
    (void)frame;
    (void)timestamp;
    return -1;
#endif
}

#if defined(_WIN32) && !defined(VENIENT_SOFTING_DRV_DISABLED)
static double softing_baudrate_to_kbps(enum can_baudrate baud)
{
    switch (baud)
    {
        case CAN_BAUD_800K:
            return 800.0;
        case CAN_BAUD_500K:
            return 500.0;
        case CAN_BAUD_250K:
            return 250.0;
        case CAN_BAUD_125K:
            return 125.0;
        case CAN_BAUD_100K:
            return 100.0;
        case CAN_BAUD_95K:
            return 95.238;
        case CAN_BAUD_83K:
            return 83.333;
        case CAN_BAUD_50K:
            return 50.0;
        case CAN_BAUD_47K:
            return 47.619;
        case CAN_BAUD_33K:
            return 33.333;
        case CAN_BAUD_20K:
            return 20.0;
        case CAN_BAUD_10K:
            return 10.0;
        case CAN_BAUD_5K:
            return 5.0;
        case CAN_BAUD_1M:
        default:
            return 1000.0;
    }
}
#endif
