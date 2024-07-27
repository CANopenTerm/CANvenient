/** @file CANvenient.h
 *
 *  An abstraction layer for multiple CAN APIs on Windows.
 *
 *  Copyright (c) 2024, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#ifndef CANVENIENT_H
#define CANVENIENT_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef CAN_MAX_CHANNEL
#  define CAN_MAX_CHANNEL 32
#endif

typedef enum can_vendor
{
    CAN_VENDOR_NONE,
    CAN_VENDOR_PEAK,
    CAN_VENDOR_IXXAT,
    CAN_VENDOR_SOCKETCAN

} can_vendor_t;

typedef struct can_message
{
    unsigned int  id;
    unsigned char is_extended;
    unsigned char data[8];
    unsigned char length;

} can_message_t;

typedef struct can_channel
{
    unsigned int id;
    can_vendor_t vendor;
    void*        handle;
    char         name[64];

} can_channel_t;

typedef struct can_handle
{
    can_channel_t channel[CAN_MAX_CHANNEL];
    unsigned int  channel_count;

} can_handle_t;

int can_init(can_handle_t** handle);
int can_open_channel(int channel, can_handle_t* handle);
int can_close_channel(int channel, can_handle_t* handle);
int can_send(int channel, unsigned int id, unsigned char* data, unsigned char length, can_handle_t* handle);
int can_receive(int channel, unsigned int* id, unsigned char* data, unsigned char* length, can_handle_t* handle);
int can_deinit(can_handle_t* handle);

#ifdef __cplusplus
}
#endif

#endif /* CANVENIENT_H */
