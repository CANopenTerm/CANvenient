#ifndef __CAN_DRV_H__
#define __CAN_DRV_H__

#include "can_types.h"
#include "can_drv_config.h"

#ifdef __WIN32__
// ****** Windows
#include <windows.h>
  #define DRV_CALLBACK_TYPE __stdcall
#else
  #include <time.h>
  #include <unistd.h>
  #include <pthread.h>
  #define DRV_CALLBACK_TYPE
#endif

#ifdef __cplusplus
  extern "C" {
#endif

/***************************************************************/
/* Define Makros                                               */
/***************************************************************/

// CAN Übertragungsgeschwindigkeit
#define CAN_10K_BIT   10    // 10 kBit/s
#define CAN_20K_BIT   20    // 20 kBit/s
#define CAN_50K_BIT   50    // 50 kBit/s
#define CAN_100K_BIT  100   // 100 kBit/s
#define CAN_125K_BIT  125   // 125 kBit/s
#define CAN_250K_BIT  250   // 250 kBit/s
#define CAN_500K_BIT  500   // 500 kBit/s
#define CAN_800K_BIT  800   // 800 kBit/s
#define CAN_1M_BIT    1000  // 1 MBit/s
#define CAN_1M5_BIT   1500  // 1,5 MBit/s
#define CAN_2M_BIT    2000  // 2 MBit/s
#define CAN_3M_BIT    3000  // 3 MBit/s
#define CAN_4M_BIT    4000  // 4 MBit/s

// Time Stamp Mode
#define TIME_STAMP_OFF        0  // keine Time-Stamps
#define TIME_STAMP_SOFT       1  // Software Time-Stamps
#define TIME_STAMP_HW_UNIX    2  // Hardware Time-Stamps, UNIX-Format
#define TIME_STAMP_HW         3  // Hardware Time-Stamps
#define TIME_STAMP_HW_SW_UNIX 4  // Hardware Time-Stamps verwenden wenn verfügbar,
                                 // ansonsten Software Time-Stamps
                                 // Ab Treiber Version 4.08!

// CAN Bus Mode
#define OP_CAN_NO_CHANGE         0  // Aktuellen Zustand nicht ändern
#define OP_CAN_START             1  // Startet den CAN-Bus
#define OP_CAN_STOP              2  // Stopt den CAN-Bus
#define OP_CAN_RESET             3  // Reset CAN Controller (BusOff löschen)
#define OP_CAN_START_LOM         4  // Startet den CAN-Bus im Silent Mode (Listen Only Mode)
#define OP_CAN_START_NO_RETRANS  5  // Startet den CAN-Bus im Automatic Retransmission disable Mode
#define OP_CAN_ECU_FLASH_MODE    6  // Start im ECU Flash Mode

#define CAN_CMD_NONE                 0x0000
#define CAN_CMD_RXD_OVERRUN_CLEAR    0x0001
#define CAN_CMD_RXD_FIFOS_CLEAR      0x0002
#define CAN_CMD_TXD_OVERRUN_CLEAR    0x0004
#define CAN_CMD_TXD_FIFOS_CLEAR      0x0008
#define CAN_CMD_HW_FILTER_CLEAR      0x0010
#define CAN_CMD_SW_FILTER_CLEAR      0x0020
#define CAN_CMD_TXD_PUFFERS_CLEAR    0x0040
// <*> neu
#define CAN_CMD_START_DATA_TRANSFER  0x1000
#define CAN_CMD_CANCEL_DATA_TRANSFER 0x2000
#define CAN_CMD_START_TEST           0xE000
#define CAN_CMD_STOP_TEST            0xF000

#define CAN_CMD_FIFOS_CLEAR         0x000F
#define CAN_CMD_ALL_CLEAR           0x0FFF


// DrvStatus
#define DRV_NOT_LOAD              0  // Die Treiber DLL wurde noch nicht geladen
#define DRV_STATUS_NOT_INIT       1  // Treiber noch nicht Initialisiert (Funktion "CanInitDrv" noch nicht aufgerufen)
#define DRV_STATUS_INIT           2  // Treiber erfolgrich Initialisiert
#define DRV_STATUS_PORT_NOT_OPEN  3  // Die Schnittstelle wurde nicht geöffnet
#define DRV_STATUS_PORT_OPEN      4  // Die Schnittstelle wurde geöffnet
#define DRV_STATUS_DEVICE_FOUND   5  // Verbindung zur Hardware wurde Hergestellt
#define DRV_STATUS_CAN_OPEN       6  // Device wurde geöffnet und erfolgreich Initialisiert
#define DRV_STATUS_CAN_RUN_TX     7  // CAN Bus RUN nur Transmitter (wird nicht verwendet !)
#define DRV_STATUS_CAN_RUN        8  // CAN Bus RUN

// CanStatus
#define CAN_STATUS_OK          0     // CAN-Controller: Ok
#define CAN_STATUS_ERROR       1     // CAN-Controller: CAN Error
#define CAN_STATUS_WARNING     2     // CAN-Controller: Error warning
#define CAN_STATUS_PASSIV      3     // CAN-Controller: Error passiv
#define CAN_STATUS_BUS_OFF     4     // CAN-Controller: Bus Off
#define CAN_STATUS_UNBEKANNT   5     // CAN-Controller: Status Unbekannt

// Neu für Low-Speed CAN, TJA1055 Fehler
#define BUS_FAILURE            0x10

// Fifo Status
#define FIFO_OK                 0    // Fifo-Status: Ok
#define FIFO_HW_OVERRUN         1    // Fifo-Status: Hardware Fifo Überlauf
#define FIFO_SW_OVERRUN         2    // Fifo-Status: Software Fifo Überlauf
#define FIFO_HW_SW_OVERRUN      3    // Fifo-Status: Hardware & Software Fifo Überlauf
#define FIFO_STATUS_UNBEKANNT   4    // Fifo-Status: Unbekannt

// Makros für SetEvent
#define EVENT_ENABLE_PNP_CHANGE          0x0001
#define EVENT_ENABLE_STATUS_CHANGE       0x0002
#define EVENT_ENABLE_RX_FILTER_MESSAGES  0x0004
#define EVENT_ENABLE_RX_MESSAGES         0x0008
#define EVENT_ENABLE_ALL                 0x00FF

#define EVENT_DISABLE_PNP_CHANGE         0x0100
#define EVENT_DISABLE_STATUS_CHANGE      0x0200
#define EVENT_DISABLE_RX_FILTER_MESSAGES 0x0400
#define EVENT_DISABLE_RX_MESSAGES        0x0800
#define EVENT_DISABLE_ALL                0xFF00

// <*> Neu
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
#define TCAN_LOG_USB               0x00008000  // <*> neu
#define TCAN_LOG_FLAG_DEBUG        0x08000000
#define TCAN_LOG_FILE_HEADER       0x10000000 // <*> neu
#define TCAN_LOG_FILE_APPEND       0x20000000 // <*> neu
#define TCAN_LOG_FLAG_WITH_TIME    0x40000000
#define TCAN_LOG_FLAG_DISABLE_SYNC 0x80000000

/***************************************************************/
/*  Typen                                                      */
/***************************************************************/

/******************************************/
/*             Device Status              */
/******************************************/
#pragma pack(push, 1)
struct TDeviceStatus
  {
  int32_t DrvStatus;         // Treiber Status (Device close / Device open / CAN Bus RUN)
  unsigned char CanStatus;   // Status des CAN Controllers (Ok / ... / Error passiv / Bus off)
  unsigned char FifoStatus;  // Fifo Status (Ok / ... / Hard. u. Soft. FIFO Überlauf)
  };
#pragma pack(pop)


#ifdef CAN_API_TRUE_FUNC

int32_t CanInitDriver(char *options);
void CanDownDriver(void);
int32_t CanSetOptions(char *options);
int32_t CanDeviceOpen(uint32_t index, char *parameter);
int32_t CanDeviceClose(uint32_t index);

int32_t CanSetMode(uint32_t index, unsigned char can_op_mode, uint16_t can_command);

int32_t CanTransmit(uint32_t index, struct TCanMsg *msg, int32_t count);
void CanTransmitClear(uint32_t index);
uint32_t CanTransmitGetCount(uint32_t index);
int32_t CanTransmitSet(uint32_t index, uint16_t  cmd, uint32_t  time);
int32_t CanReceive(uint32_t index, struct TCanMsg *msg, int32_t count);
void CanReceiveClear(uint32_t index);
uint32_t  CanReceiveGetCount(uint32_t index);

int32_t CanSetSpeed(uint32_t index, uint16_t speed);
int32_t CanSetSpeedUser(uint32_t index, uint32_t value);
char *CanDrvInfo(void);
char *CanDrvHwInfo(uint32_t index);
int32_t CanSetFilter(uint32_t index, struct TMsgFilter *msg_filter);
int32_t CanGetDeviceStatus(uint32_t index, struct TDeviceStatus *status);
void CanSetPnPEventCallback(void (DRV_CALLBACK_TYPE *event)(uint32_t index, int32_t status));
void CanSetStatusEventCallback(void (DRV_CALLBACK_TYPE *event)
  (uint32_t  index, struct TDeviceStatus *device_status));
void CanSetRxEventCallback(void (DRV_CALLBACK_TYPE *event)(uint32_t index,
  struct TCanMsg *msg, int32_t count));

void CanSetEvents(uint16_t  events);
uint32_t CanEventStatus(void);

#endif


#if !(defined(CAN_API_TRUE_FUNC)) || defined(CAN_DRV_INCLUDE)
/***************************************************************/
/*  Funktionstypen                                             */
/***************************************************************/
typedef int32_t (DRV_CALLBACK_TYPE *TCanInitDriver)(char *options);
typedef void (DRV_CALLBACK_TYPE *TCanDownDriver)(void);
typedef int32_t (DRV_CALLBACK_TYPE *TCanSetOptions)(char *options);
typedef int32_t (DRV_CALLBACK_TYPE *TCanDeviceOpen)(uint32_t index, char *parameter);
typedef int32_t (DRV_CALLBACK_TYPE *TCanDeviceClose)(uint32_t index);

typedef int32_t (DRV_CALLBACK_TYPE *TCanSetMode)(uint32_t index, unsigned char can_op_mode,
                 uint16_t  can_command);

typedef int32_t (DRV_CALLBACK_TYPE *TCanTransmit)(uint32_t index, struct TCanMsg *msg, int32_t count);
typedef void (DRV_CALLBACK_TYPE *TCanTransmitClear)(uint32_t index);
typedef uint32_t  (DRV_CALLBACK_TYPE *TCanTransmitGetCount)(uint32_t index);
typedef int32_t (DRV_CALLBACK_TYPE *TCanTransmitSet)(uint32_t index, uint16_t cmd, uint32_t time);
typedef int32_t (DRV_CALLBACK_TYPE *TCanReceive)(uint32_t index, struct TCanMsg *msg, int32_t count);
typedef void (DRV_CALLBACK_TYPE *TCanReceiveClear)(uint32_t index);
typedef uint32_t  (DRV_CALLBACK_TYPE *TCanReceiveGetCount)(uint32_t index);

typedef int32_t (DRV_CALLBACK_TYPE *TCanSetSpeed)(uint32_t index, uint16_t speed);
typedef int32_t (DRV_CALLBACK_TYPE *TCanSetSpeedUser)(uint32_t index, uint32_t value);
typedef char * (DRV_CALLBACK_TYPE *TCanDrvInfo)(void);
typedef char * (DRV_CALLBACK_TYPE *TCanDrvHwInfo)(uint32_t index);
typedef int32_t (DRV_CALLBACK_TYPE *TCanSetFilter)(uint32_t index, struct TMsgFilter *msg_filter);

typedef int32_t (DRV_CALLBACK_TYPE *TCanGetDeviceStatus)(uint32_t index, struct TDeviceStatus *status);

typedef void (DRV_CALLBACK_TYPE *TCanSetPnPEventCallback)(void (DRV_CALLBACK_TYPE *event)
   (uint32_t index, int32_t status));
typedef void (DRV_CALLBACK_TYPE *TCanSetStatusEventCallback)(void (DRV_CALLBACK_TYPE *event)
   (uint32_t index, struct TDeviceStatus *device_status));
typedef void (DRV_CALLBACK_TYPE *TCanSetRxEventCallback)(void (DRV_CALLBACK_TYPE *event)
   (uint32_t index, struct TCanMsg *msg, int32_t count));

typedef void (DRV_CALLBACK_TYPE *TCanSetEvents)(uint16_t events);
typedef uint32_t (DRV_CALLBACK_TYPE *TCanEventStatus)(void);

#endif


#ifndef CAN_API_TRUE_FUNC
/***************************************************************/
/*  Tiny-CAN API Funktionen                                    */
/***************************************************************/

extern TCanInitDriver CanInitDriver;
extern TCanDownDriver CanDownDriver;
extern TCanSetOptions CanSetOptions;
extern TCanDeviceOpen CanDeviceOpen;
extern TCanDeviceClose CanDeviceClose;

extern TCanSetMode CanSetMode;

extern TCanTransmit CanTransmit;
extern TCanTransmitClear CanTransmitClear;
extern TCanTransmitGetCount CanTransmitGetCount;
extern TCanTransmitSet CanTransmitSet;
extern TCanReceive CanReceive;
extern TCanReceiveClear CanReceiveClear;
extern TCanReceiveGetCount CanReceiveGetCount;

extern TCanSetSpeed CanSetSpeed;
extern TCanSetSpeedUser CanSetSpeedUser;

extern TCanDrvInfo CanDrvInfo;
extern TCanDrvHwInfo CanDrvHwInfo;
extern TCanSetFilter CanSetFilter;

extern TCanGetDeviceStatus CanGetDeviceStatus;

extern TCanSetPnPEventCallback CanSetPnPEventCallback;
extern TCanSetStatusEventCallback CanSetStatusEventCallback;
extern TCanSetRxEventCallback CanSetRxEventCallback;

extern TCanSetEvents CanSetEvents;
extern TCanEventStatus CanEventStatus;

#endif

/***************************************************************/
/*  Funktionen Treiber laden/entladen                          */
/***************************************************************/
int32_t LoadDriver(const char *file_name);
void UnloadDriver(void);

#define _CAN_DRV_EX_INCLUDES_
#include "can_drv_ex.h"
#undef _CAN_DRV_EX_INCLUDES_

#ifdef __cplusplus
  }
#endif

#endif
