/** @file CANvenient_TinyCan.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, MHS-Elektronik GmbH & Co. KG, Klaus Demlehner.
 *  Copyright (c) 2026, Michael Fitzmayer.
 *  All rights reserved.
 *
 *  SPDX-License-Identifier: MIT
 *
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CANvenient.h"
#include "CANvenient_internal.h"

#include "can_drv.h"

typedef struct _TTinyDevice TTinyDevice;

struct _TTinyDevice
  {
  uint32_t DeviceIndex;
  char Snr[20];
  };


/*
#define TCAN_LOG_FLAG_MESSAGE      0x00000001
#define TCAN_LOG_FLAG_STATUS       0x00000002
#define TCAN_LOG_FLAG_RX_MSG       0x00000004
#define TCAN_LOG_FLAG_TX_MSG       0x00000008
#define TCAN_LOG_FLAG_API_CALL     0x00000010
#define TCAN_LOG_API_CALL_RX       0x00000020
#define TCAN_LOG_API_CALL_TX       0x00000040
#define TCAN_LOG_API_CALL_STATUS   0x00000080
#define TCAN_LOG_FLAG_ERROR        0x00000100
#define TCAN_LOG_FLAG_WARN         0x00000200
#define TCAN_LOG_FLAG_ERR_MSG      0x00000400
#define TCAN_LOG_FLAG_OV_MSG       0x00000800
#define TCAN_LOG_USB               0x00008000
#define TCAN_LOG_FLAG_DEBUG        0x08000000
#define TCAN_LOG_FILE_HEADER       0x10000000
#define TCAN_LOG_FILE_APPEND       0x20000000
#define TCAN_LOG_FLAG_WITH_TIME    0x40000000
#define TCAN_LOG_FLAG_DISABLE_SYNC 0x80000000
*/

// TimeStampMode
//   0 = Disabled
//   1 = Software Time Stamps
//   2 = Hardware Time Stamps, UNIX-Format
//   3 = Hardware Time Stamps
//   4 = Hardware Time Stamp wenn verfügbar, ansonsten Software Time Stamps
#ifdef ENABLE_LOG
  #pragma message("!!!!!! WARNING: CAN-API driver log is enabled !!!!!!")

  #define TREIBER_INIT_PARAM "logfile=tinycan.log;logflags=0x880000F3;FdMode=0;CanCallThread=0;TimeStampMode=4"
#else
  #define TREIBER_INIT_PARAM "FdMode=0;CanCallThread=0;TimeStampMode=4"
#endif

#define DEVICE_OPEN_PARAM "AutoStopCan=1"

static uint32_t TinyCanDriverInit = 0;


struct TCanVenientToTCanBaudrate
  {
  enum can_baudrate CanVenientBaud;
  uint16_t TCanBaudrate;
  };

static const struct TCanVenientToTCanBaudrate CanVenientToTCanBaudrate[] = {
  {CAN_BAUD_1M,    CAN_1M_BIT},
  {CAN_BAUD_800K,  CAN_800K_BIT},
  {CAN_BAUD_500K,  CAN_500K_BIT},
  {CAN_BAUD_250K,  CAN_250K_BIT},
  {CAN_BAUD_125K,  CAN_125K_BIT},
  {CAN_BAUD_100K,  CAN_100K_BIT},
  {CAN_BAUD_95K,   0xFFFF},
  {CAN_BAUD_83K,   0xFFFF},
  {CAN_BAUD_50K,   CAN_50K_BIT},
  {CAN_BAUD_47K,   0xFFFF},
  {CAN_BAUD_33K,   0xFFFF},
  {CAN_BAUD_20K,   CAN_20K_BIT},
  {CAN_BAUD_10K,   CAN_10K_BIT},
  {CAN_BAUD_5K,    0xFFFF},
  {0, 0}};


#define CAN_ERROR_TCAN        -1
#define CAN_ERROR_QXMTFULL    -2
#define CAN_ERROR_QRCVEMPTY   -3

#define CAN_ERROR_QOVERRUN    -4
#define CAN_ERROR_BUSPASSIVE  -5
#define CAN_ERROR_BUSOFF      -6

#define CAN_ERROR_NODRIVER    -5
#define CAN_ERROR_REGTEST     -6
#define CAN_ERROR_RESOURCE    -7


struct TCanVenientMhsError
  {
  int32_t CanError;
  const char *ErrorMsg;
  };


static const struct TCanVenientMhsError CanVenientMhsError[] = {
  {CAN_ERROR_TCAN, "Tiny-CAN API error."},
  {CAN_ERROR_QXMTFULL, "Transmit queue is full."},
  {CAN_ERROR_QRCVEMPTY, "Receive queue is empty."},

  {CAN_ERROR_QOVERRUN, "Receive queue was read too late."},
  {CAN_ERROR_BUSPASSIVE, "Bus error: the CAN controller is error passive."},
  {CAN_ERROR_BUSOFF, "Bus error: the CAN controller is in bus-off state."},

  {CAN_ERROR_NODRIVER, "Driver not loaded."},

  {CAN_ERROR_REGTEST, "Test of the CAN controller hardware registers failed (no hardware found)."},

  {CAN_ERROR_RESOURCE, "Resource (FIFO, Client, timeout) cannot be created."},
  {0, NULL}};


static const char *lookup_error_string(int32_t can_error, int32_t tcan_error)
{
const struct TCanVenientMhsError *err_list;
const char *error_str;
(void)tcan_error;

if (can_error)
  error_str = "No error. Success.";
else
  {
  for (err_list = CanVenientMhsError; (error_str = err_list->ErrorMsg); err_list++)
    {
    if (can_error == err_list->CanError)
      break;
    }
  }
if (!error_str)
  error_str = "Unknown error.";
return(error_str);
}


static char *mhs_strdup(const char *str)
{
size_t len;
char *new_str;

if (str)
  {
  len = strlen(str) + 1;
  new_str = (char *)malloc(len);
  if (!new_str)
    return(NULL);
  memcpy(new_str, str, len);
  return(new_str);
  }
else
  return(NULL);
}


int tinycan_find_interfaces(void)
{
int res;
uint32_t k, idx, already_registered;
struct TCanDevicesList *can_dev_list;
int32_t num_devs, i;
char dev_name[100];
TTinyDevice *tiny_device;

res = 0;
if (!TinyCanDriverInit)
  {
  if (LoadDriver(NULL) >= 0)
    {
    if (CanInitDriver(TREIBER_INIT_PARAM) >= 0)
      TinyCanDriverInit = 1;
    }
  }
if ((num_devs = CanExGetDeviceList(&can_dev_list, 0)) <= 0)
  {
  set_error_reason("No Tiny-CAN hardwre found");
  return(-1);
  }
for (i = 0; i < num_devs; i++)
  {
  snprintf(dev_name, sizeof(dev_name), "%s (S/N: %s)", can_dev_list[i].Description, can_dev_list[i].SerialNumber);

  already_registered = 0;
  for (k = 0; k < CAN_MAX_INTERFACES; k++)
    {
    if (can_interface[k].name && strcmp(can_interface[k].name, dev_name) == 0)
      {
      already_registered = 1;
      break;
      }
    }
  if (already_registered)
    continue;

  if (0 != find_free_interface_slot(&idx))
    break; /* No free slot available: stop. */

  if (!(tiny_device = malloc(sizeof(TTinyDevice))))
    {
    res = -1;
    break;
    }
  can_interface[idx].internal = tiny_device;
  if (!(can_interface[idx].name = mhs_strdup(dev_name)))
    {
    free(can_interface[idx].internal);
    can_interface[idx].internal = NULL;
    res = -1;
    }

  tiny_device->DeviceIndex = INDEX_INVALID;
  snprintf(tiny_device->Snr, 20, "%s", can_dev_list[i].SerialNumber);

  can_interface[idx].vendor = CAN_VENDOR_MHS;
  can_interface[idx].opened = 0;
  can_interface[idx].baudrate = CAN_BAUD_1M;
  }
CanExDataFree((void **)&can_dev_list);
if (res < 0)
  set_error_reason("Memory allocation failed.");
return(res);
}


int tinycan_open(int index)
{
int32_t err;
TTinyDevice *tiny_device;
const struct TCanVenientToTCanBaudrate *b;
uint16_t tcan_baud;
//char dev_open_str[200]; <*>

err = 0;
if (!(tiny_device = (TTinyDevice *)can_interface[index].internal))
  return(-1);
for (b = &CanVenientToTCanBaudrate[0]; (tcan_baud = b->TCanBaudrate); b++)
  {
  if (b->CanVenientBaud == can_interface[index].baudrate)
    break;
  }
if (!tcan_baud)
  err = -1;
if (tcan_baud == 0xFFFF)
  err = -1;
if ((!err) && (tiny_device->DeviceIndex == INDEX_INVALID))
  err = CanExCreateDevice(&tiny_device->DeviceIndex, "CanRxDFifoSize=16384");
if (!err)
  {
  (void)CanExSetAsUWord(tiny_device->DeviceIndex, "CanSpeed1", tcan_baud);
  (void)CanExSetAsString(tiny_device->DeviceIndex, "Snr", tiny_device->Snr);
  }
//snprintf(dev_open_str, sizeof(dev_open_str), "%s;Snr=%s", DEVICE_OPEN_PARAM, tiny_device->Snr);<*>
if (!err)
  err = CanDeviceOpen(tiny_device->DeviceIndex, DEVICE_OPEN_PARAM);
  //err = CanDeviceOpen(tiny_device->DeviceIndex, dev_open_str);<*>
if (!err)
  err = CanSetMode(tiny_device->DeviceIndex, OP_CAN_START, CAN_CMD_ALL_CLEAR);
if (err)
  return(-1);
else
  {
  can_interface[index].opened = 1;
  return(0);
  }
}


void tinycan_close(int index)
{
TTinyDevice *tiny_device;

if (!(tiny_device = (TTinyDevice *)can_interface[index].internal))
  return;
(void)CanDeviceClose(tiny_device->DeviceIndex);
can_interface[index].opened = 0; // <*>
}


int tinycan_update(int index)
{
uint32_t dev_idx;
struct TDeviceStatus status;
/*int32_t can_error;

can_error = 0;*/
dev_idx = ((TTinyDevice *)can_interface[index].internal)->DeviceIndex;
CanGetDeviceStatus(dev_idx, &status);
if (status.DrvStatus >= DRV_STATUS_CAN_OPEN)
  {
  /*if (status.FifoStatus & FIFO_HW_SW_OVERRUN)
    can_error = CAN_ERROR_QOVERRUN;
  if (status.CanStatus == CAN_STATUS_BUS_OFF)
    can_error = CAN_ERROR_BUSOFF;
  else if (status.CanStatus == CAN_STATUS_PASSIV)
    can_error = CAN_ERROR_BUSPASSIVE;*/
  }
else
  {
  can_release(index);
  return(-1);
  }
return(0);
}


int tinycan_set_baudrate(int index, enum can_baudrate baud)
{
can_close(index);
can_interface[index].baudrate = baud;
return can_open(index, baud);
}


int tinycan_send(int index, struct can_frame* frame)
{
uint32_t dev_idx, len;
struct TCanMsg tcan_msg;
int32_t can_error, tcan_res;

can_error = 0;
dev_idx = ((TTinyDevice *)can_interface[index].internal)->DeviceIndex;
len = frame->can_dlc;
tcan_msg.Flags.Long = len;
tcan_msg.Id = frame->can_id;
if (len)
  memcpy(tcan_msg.MsgData, frame->data, len);
if ((tcan_res = CanTransmit(dev_idx, &tcan_msg, 1)) < 0)
  can_error = CAN_ERROR_TCAN;
else if (tcan_res == 0)
  can_error = CAN_ERROR_QXMTFULL;
if (can_error)
  {
  set_error_reason(lookup_error_string(can_error, tcan_res));
  return(-1);
  }
return(0);
}


int tinycan_recv(int index, struct can_frame* frame, u64* timestamp)
{
uint32_t dev_idx, len;
struct TCanMsg tcan_msg;
int32_t can_error, tcan_res;

can_error = 0;
dev_idx = ((TTinyDevice *)can_interface[index].internal)->DeviceIndex;
if ((tcan_res = CanReceive(dev_idx, &tcan_msg, 1)) == 1)
  {
  if (!tcan_msg.MsgRTR)
    {
    len = tcan_msg.MsgLen;
    frame->can_id = tcan_msg.Id;
    frame->can_dlc = len;
    if (len)
      memcpy(frame->data, tcan_msg.MsgData, len);
    *timestamp = 0;  // T.D.
    }
  else
    can_error = CAN_ERROR_QRCVEMPTY;
  }
else if (tcan_res == 0)
  can_error = CAN_ERROR_QRCVEMPTY;
else
  can_error = CAN_ERROR_TCAN;
if (can_error)
  {
  set_error_reason(lookup_error_string(can_error, tcan_res));
  return(-1);
  }
return(0);
}


