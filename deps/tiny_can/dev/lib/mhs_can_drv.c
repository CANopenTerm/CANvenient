/***************************************************************************
                          Interface zur Tiny-CAN API
                             -------------------
    begin             : 11.10.2011
    last modify       : 04.05.2023    
    copyright         : (C) 2011 - 2023 by MHS-Elektronik GmbH & Co. KG, Germany
    author            : Klaus Demlehner, klaus@mhs-elektronik.de
 ***************************************************************************/
#define CAN_DRV_INCLUDE

#include "can_drv.h"

#ifdef __WIN32__
  #include <string.h>
  #include <stdlib.h>
  #if defined(MHS_DRV_DEBUG_OUTPUT)
    #include <stdio.h>
    #include <string.h>
  #endif
#else
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <errno.h>
  #include <dlfcn.h>
  #if defined(LINUX_HAVE_GET_API_DRIVER_PATH)
    #include <stdarg.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <limits.h> /* PATH_MAX */
  #endif
  #if defined(MHS_DRV_DEBUG_OUTPUT) || defined(LINUX_HAVE_GET_API_DRIVER_PATH)
    #include <string.h>
  #endif
#endif


#ifdef CAN_API_TRUE_FUNC
static TCanInitDriver PCanInitDriver = NULL;
static TCanDownDriver PCanDownDriver = NULL;
static TCanSetOptions PCanSetOptions = NULL;
static TCanDeviceOpen PCanDeviceOpen = NULL;
static TCanDeviceClose PCanDeviceClose = NULL;
static TCanSetMode PCanSetMode = NULL;
static TCanTransmit PCanTransmit = NULL;
static TCanTransmitClear PCanTransmitClear = NULL;
static TCanTransmitGetCount PCanTransmitGetCount =  NULL;
static TCanTransmitSet PCanTransmitSet = NULL;
static TCanReceive PCanReceive = NULL;
static TCanReceiveClear PCanReceiveClear = NULL;
static TCanReceiveGetCount PCanReceiveGetCount = NULL;
static TCanSetSpeed PCanSetSpeed = NULL;
static TCanSetSpeedUser PCanSetSpeedUser = NULL;
static TCanDrvInfo PCanDrvInfo = NULL;
static TCanDrvHwInfo PCanDrvHwInfo = NULL;
static TCanSetFilter PCanSetFilter = NULL;
static TCanGetDeviceStatus PCanGetDeviceStatus = NULL;
static TCanSetPnPEventCallback PCanSetPnPEventCallback = NULL;
static TCanSetStatusEventCallback PCanSetStatusEventCallback = NULL;
static TCanSetRxEventCallback PCanSetRxEventCallback = NULL;
static TCanSetEvents PCanSetEvents = NULL;
static TCanEventStatus PCanEventStatus = NULL;
// EX-Funktionen
static TCanExGetDeviceCount PCanExGetDeviceCount = NULL;
static TCanExGetDeviceList PCanExGetDeviceList = NULL;
static TCanExGetDeviceListPerform PCanExGetDeviceListPerform = NULL;
static TCanExGetDeviceListGet PCanExGetDeviceListGet = NULL;
static TCanExGetDeviceInfo PCanExGetDeviceInfo = NULL;
static TCanExGetDeviceInfoPerform PCanExGetDeviceInfoPerform = NULL;
static TCanExGetDeviceInfoGet PCanExGetDeviceInfoGet = NULL;
static TCanExDataFree PCanExDataFree = NULL;
static TCanExCreateDevice PCanExCreateDevice = NULL;
static TCanExDestroyDevice PCanExDestroyDevice = NULL;
static TCanExCreateFifo PCanExCreateFifo = NULL;
static TCanExBindFifo PCanExBindFifo = NULL;
static TCanExCreateEvent PCanExCreateEvent = NULL;
static TCanExSetObjEvent PCanExSetObjEvent = NULL;
static TCanExSetEvent PCanExSetEvent = NULL;
static TCanExSetEventAll PCanExSetEventAll = NULL;
static TCanExResetEvent PCanExResetEvent = NULL;
static TCanExWaitForEvent PCanExWaitForEvent = NULL;
static TCanExInitDriver PCanExInitDriver = NULL;
static TCanExSetOptions PCanExSetOptions = NULL;
static TCanExSetAsByte PCanExSetAsByte = NULL;
static TCanExSetAsWord PCanExSetAsWord = NULL;
static TCanExSetAsLong PCanExSetAsLong = NULL;
static TCanExSetAsUByte PCanExSetAsUByte = NULL;
static TCanExSetAsUWord PCanExSetAsUWord = NULL;
static TCanExSetAsULong PCanExSetAsULong = NULL;
static TCanExSetAsString PCanExSetAsString = NULL;
static TCanExGetAsByte PCanExGetAsByte = NULL;
static TCanExGetAsWord PCanExGetAsWord = NULL;
static TCanExGetAsLong PCanExGetAsLong = NULL;
static TCanExGetAsUByte PCanExGetAsUByte = NULL;
static TCanExGetAsUWord PCanExGetAsUWord = NULL;
static TCanExGetAsULong PCanExGetAsULong = NULL;
static TCanExGetAsString PCanExGetAsString = NULL;
static TCanExGetAsStringCopy PCanExGetAsStringCopy = NULL;
// **** CAN-FD
static TCanFdTransmit PCanFdTransmit = NULL;
static TCanFdReceive PCanFdReceive = NULL;
// **** <*> neu
static TCanExSetAsData PCanExSetAsData = NULL;
static TCanExSetDataPtr PCanExSetDataPtr = NULL;
static TCanExSetDataBlockCB PCanExSetDataBlockCB = NULL;

static TCanExGetInfoList PCanExGetInfoList = NULL;
static TCanExGetInfoListPerform PCanExGetInfoListPerform = NULL;
static TCanExGetInfoListGet PCanExGetInfoListGet = NULL;

static TMhsCanGetApiHandle PMhsCanGetApiHandle = NULL;
#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu
static TCanExIoOpen PCanExIoOpen = NULL;
static TCanExIoClose PCanExIoClose = NULL;
static TCanExIoWrite PCanExIoWrite = NULL;
static TCanExIoRead PCanExIoRead = NULL;
#endif

#else
int32_t DRV_CALLBACK_TYPE DefCanInitDriver(char *options);
void DRV_CALLBACK_TYPE DefCanDownDriver(void);
int32_t DRV_CALLBACK_TYPE DefCanSetOptions(char *options);
int32_t DRV_CALLBACK_TYPE DefCanDeviceOpen(uint32_t  index, char *parameter);
int32_t DRV_CALLBACK_TYPE DefCanDeviceClose(uint32_t  index);

int32_t DRV_CALLBACK_TYPE DefCanSetMode(uint32_t  index, unsigned char can_op_mode, uint16_t  can_command);

int32_t DRV_CALLBACK_TYPE DefCanTransmit(uint32_t  index, struct TCanMsg *msg, int32_t count);
void DRV_CALLBACK_TYPE DefCanTransmitClear(uint32_t  index);
uint32_t  DRV_CALLBACK_TYPE DefCanTransmitGetCount(uint32_t  index);
int32_t DRV_CALLBACK_TYPE DefCanTransmitSet(uint32_t  index, uint16_t  cmd,
  uint32_t  time);
int32_t DRV_CALLBACK_TYPE DefCanReceive(uint32_t  index, struct TCanMsg *msg, int32_t count);
void DRV_CALLBACK_TYPE DefCanReceiveClear(uint32_t  index);
uint32_t  DRV_CALLBACK_TYPE DefCanReceiveGetCount(uint32_t  index);

int32_t DRV_CALLBACK_TYPE DefCanSetSpeed(uint32_t  index, uint16_t  speed);
int32_t DRV_CALLBACK_TYPE DefCanSetSpeedUser(uint32_t  index, uint32_t  value);
char * DRV_CALLBACK_TYPE DefCanDrvInfo(void);
char * DRV_CALLBACK_TYPE DefCanDrvHwInfo(uint32_t index);
int32_t DRV_CALLBACK_TYPE DefCanSetFilter(uint32_t index, struct TMsgFilter *msg_filter);
int32_t DRV_CALLBACK_TYPE DefCanGetDeviceStatus(uint32_t  index, struct TDeviceStatus *status);
void DRV_CALLBACK_TYPE DefCanSetPnPEventCallback(void (DRV_CALLBACK_TYPE *event)(uint32_t  index, int32_t status));
void DRV_CALLBACK_TYPE DefCanSetStatusEventCallback(void (DRV_CALLBACK_TYPE *event)
  (uint32_t  index, struct TDeviceStatus *device_status));
void DRV_CALLBACK_TYPE DefCanSetRxEventCallback(void (DRV_CALLBACK_TYPE *event)(uint32_t  index,
  struct TCanMsg *msg, int32_t count));

void DRV_CALLBACK_TYPE DefCanSetEvents(uint16_t  events);
uint32_t DRV_CALLBACK_TYPE DefCanEventStatus(void);

// EX-Funktionen
int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceCount(int32_t flags);
int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceList(struct TCanDevicesList **devices_list, int32_t flags);
int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceListPerform(int32_t flags);
int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceListGet(struct TCanDevicesList *item);
int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceInfo(uint32_t index, struct TCanDeviceInfo *device_info,
  struct TCanInfoVar **hw_info, uint32_t *hw_info_size);
int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceInfoPerform(uint32_t index, struct TCanDeviceInfo *device_info);
int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceInfoGet(struct TCanInfoVar *item);
void DRV_CALLBACK_TYPE DefCanExDataFree(void **data);
int32_t DRV_CALLBACK_TYPE DefCanExCreateDevice(uint32_t *index, char *options);
int32_t DRV_CALLBACK_TYPE DefCanExDestroyDevice(uint32_t *index);
int32_t DRV_CALLBACK_TYPE DefCanExCreateFifo(uint32_t index, uint32_t size, TMhsEvent *event_obj, uint32_t event, uint32_t channels);
int32_t DRV_CALLBACK_TYPE DefCanExBindFifo(uint32_t fifo_index, uint32_t device_index, uint32_t bind);
TMhsEvent * DRV_CALLBACK_TYPE DefCanExCreateEvent(void);
int32_t DRV_DRV_CALLBACK_TYPE_TYPE DefCanExSetObjEvent(uint32_t index, uint32_t source, TMhsEvent *event_obj, uint32_t event);
void DRV_CALLBACK_TYPE DefCanExSetEvent(TMhsEvent *event_obj, uint32_t event);
void DRV_CALLBACK_TYPE DefCanExSetEventAll(uint32_t event);
void DRV_CALLBACK_TYPE DefCanExResetEvent(TMhsEvent *event_obj, uint32_t event);
uint32_t DRV_CALLBACK_TYPE DefCanExWaitForEvent(TMhsEvent *event_obj, uint32_t timeout);
int32_t DRV_CALLBACK_TYPE DefCanExInitDriver(char *options);
int32_t DRV_CALLBACK_TYPE DefCanExSetOptions(uint32_t index, char *options);
int32_t DRV_CALLBACK_TYPE DefCanExSetAsByte(uint32_t index, char *name, char value);
int32_t DRV_CALLBACK_TYPE DefCanExSetAsWord(uint32_t index, char *name, int16_t value);
int32_t DRV_CALLBACK_TYPE DefCanExSetAsLong(uint32_t index, char *name, int32_t value);
int32_t DRV_CALLBACK_TYPE DefCanExSetAsUByte(uint32_t index, char *name, unsigned char value);
int32_t DRV_CALLBACK_TYPE DefCanExSetAsUWord(uint32_t index, char *name, uint16_t value);
int32_t DRV_CALLBACK_TYPE DefCanExSetAsULong(uint32_t index, char *name, uint32_t value);
int32_t DRV_CALLBACK_TYPE DefCanExSetAsString(uint32_t index, char *name, char *value);
int32_t DRV_CALLBACK_TYPE DefCanExGetAsByte(uint32_t index, char *name, char *value);
int32_t DRV_CALLBACK_TYPE DefCanExGetAsWord(uint32_t index, char *name, int16_t *value);
int32_t DRV_CALLBACK_TYPE DefCanExGetAsLong(uint32_t index, char *name, int32_t *value);
int32_t DRV_CALLBACK_TYPE DefCanExGetAsUByte(uint32_t index, char *name, unsigned char *value);
int32_t DRV_CALLBACK_TYPE DefCanExGetAsUWord(uint32_t index, char *name, uint16_t *value);
int32_t DRV_CALLBACK_TYPE DefCanExGetAsULong(uint32_t index, char *name, uint32_t *value);
int32_t DRV_CALLBACK_TYPE DefCanExGetAsString(uint32_t index, char *name, char **str);
int32_t DRV_CALLBACK_TYPE DefCanExGetAsStringCopy(uint32_t index, char *name, char *dest, uint32_t *dest_size);
// **** CAN-FD
int32_t DRV_CALLBACK_TYPE DefCanFdTransmit(uint32_t index, struct TCanFdMsg *fd_msg, int32_t count);
int32_t DRV_CALLBACK_TYPE DefCanFdReceive(uint32_t index, struct TCanFdMsg *fd_msg, int32_t count);
// **** <*> neu
int32_t DRV_CALLBACK_TYPE DefCanExSetAsData(uint32_t index, char *name, void *data, uint32_t size);
int32_t DRV_CALLBACK_TYPE DefCanExSetDataPtr(uint32_t index, char *name, void *data_ptr, uint32_t size);
int32_t DRV_CALLBACK_TYPE DefCanExSetDataBlockCB(uint32_t index, TCanGetDataBlockCB func, void *user_data);

int32_t DRV_CALLBACK_TYPE DefCanExGetInfoList(uint32_t dev_idx, const char *name, struct TCanInfoVarList **devices_info, int32_t flags);
int32_t DRV_CALLBACK_TYPE DefCanExGetInfoListPerform(uint32_t dev_idx, const char *name, int32_t flags);
int32_t DRV_CALLBACK_TYPE DefCanExGetInfoListGet(uint32_t list_idx, struct TCanInfoVar *item);
int32_t DRV_CALLBACK_TYPE DefMhsCanGetApiHandle(void **api_handle);

#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu
int32_t DRV_CALLBACK_TYPE CanExIoOpen(uint32_t index, const char *name, uint32_t *io_id);
int32_t DRV_CALLBACK_TYPE CanExIoClose(uint32_t io_id);
int32_t DRV_CALLBACK_TYPE CanExIoWrite(uint32_t io_id, void *data, uint32_t *size, uint32_t timeout);
int32_t DRV_CALLBACK_TYPE CanExIoRead(uint32_t io_id, void *data, uint32_t *size, uint32_t timeout);
#endif

TCanInitDriver CanInitDriver = DefCanInitDriver;
TCanDownDriver CanDownDriver = DefCanDownDriver;
TCanSetOptions CanSetOptions = DefCanSetOptions;
TCanDeviceOpen CanDeviceOpen = DefCanDeviceOpen;
TCanDeviceClose CanDeviceClose = DefCanDeviceClose;
TCanSetMode CanSetMode = DefCanSetMode;
TCanTransmit CanTransmit = DefCanTransmit;
TCanTransmitClear CanTransmitClear = DefCanTransmitClear;
TCanTransmitGetCount CanTransmitGetCount = DefCanTransmitGetCount;
TCanTransmitSet CanTransmitSet = DefCanTransmitSet;
TCanReceive CanReceive = DefCanReceive;
TCanReceiveClear CanReceiveClear = DefCanReceiveClear;
TCanReceiveGetCount CanReceiveGetCount = DefCanReceiveGetCount;
TCanSetSpeed CanSetSpeed = DefCanSetSpeed;
TCanSetSpeedUser CanSetSpeedUser = DefCanSetSpeedUser;
TCanDrvInfo CanDrvInfo = DefCanDrvInfo;
TCanDrvHwInfo CanDrvHwInfo = DefCanDrvHwInfo;
TCanSetFilter CanSetFilter = DefCanSetFilter;
TCanGetDeviceStatus CanGetDeviceStatus = DefCanGetDeviceStatus;
TCanSetPnPEventCallback CanSetPnPEventCallback = DefCanSetPnPEventCallback;
TCanSetStatusEventCallback CanSetStatusEventCallback = DefCanSetStatusEventCallback;
TCanSetRxEventCallback CanSetRxEventCallback = DefCanSetRxEventCallback;
TCanSetEvents CanSetEvents = DefCanSetEvents;
TCanEventStatus CanEventStatus = DefCanEventStatus;

// EX-Funktionen
TCanExGetDeviceCount CanExGetDeviceCount = DefCanExGetDeviceCount;
TCanExGetDeviceList CanExGetDeviceList = DefCanExGetDeviceList;
TCanExGetDeviceListPerform CanExGetDeviceListPerform = DefCanExGetDeviceListPerform;
TCanExGetDeviceListGet CanExGetDeviceListGet = DefCanExGetDeviceListGet;
TCanExGetDeviceInfo CanExGetDeviceInfo = DefCanExGetDeviceInfo;
TCanExGetDeviceInfoPerform CanExGetDeviceInfoPerform = DefCanExGetDeviceInfoPerform;
TCanExGetDeviceInfoGet CanExGetDeviceInfoGet = DefCanExGetDeviceInfoGet;
TCanExDataFree CanExDataFree = DefCanExDataFree;
TCanExCreateDevice CanExCreateDevice = DefCanExCreateDevice;
TCanExDestroyDevice CanExDestroyDevice = DefCanExDestroyDevice;
TCanExCreateFifo CanExCreateFifo = DefCanExCreateFifo;
TCanExBindFifo CanExBindFifo = DefCanExBindFifo;
TCanExCreateEvent CanExCreateEvent = DefCanExCreateEvent;
TCanExSetObjEvent CanExSetObjEvent = DefCanExSetObjEvent;
TCanExSetEvent CanExSetEvent = DefCanExSetEvent;
TCanExSetEventAll CanExSetEventAll = DefCanExSetEventAll;
TCanExResetEvent CanExResetEvent = DefCanExResetEvent;
TCanExWaitForEvent CanExWaitForEvent = DefCanExWaitForEvent;
TCanExInitDriver CanExInitDriver = DefCanExInitDriver;
TCanExSetOptions CanExSetOptions = DefCanExSetOptions;
TCanExSetAsByte CanExSetAsByte = DefCanExSetAsByte;
TCanExSetAsWord CanExSetAsWord = DefCanExSetAsWord;
TCanExSetAsLong CanExSetAsLong = DefCanExSetAsLong;
TCanExSetAsUByte CanExSetAsUByte = DefCanExSetAsUByte;
TCanExSetAsUWord CanExSetAsUWord = DefCanExSetAsUWord;
TCanExSetAsULong CanExSetAsULong = DefCanExSetAsULong;
TCanExSetAsString CanExSetAsString = DefCanExSetAsString;
TCanExGetAsByte CanExGetAsByte = DefCanExGetAsByte;
TCanExGetAsWord CanExGetAsWord = DefCanExGetAsWord;
TCanExGetAsLong CanExGetAsLong = DefCanExGetAsLong;
TCanExGetAsUByte CanExGetAsUByte = DefCanExGetAsUByte;
TCanExGetAsUWord CanExGetAsUWord = DefCanExGetAsUWord;
TCanExGetAsULong CanExGetAsULong = DefCanExGetAsULong;
TCanExGetAsString CanExGetAsString = DefCanExGetAsString;
TCanExGetAsStringCopy CanExGetAsStringCopy = DefCanExGetAsStringCopy;
// **** CAN-FD
TCanFdTransmit CanFdTransmit = DefCanFdTransmit;
TCanFdReceive CanFdReceive = DefCanFdReceive;
// <*> neu
TCanExSetAsData CanExSetAsData = DefCanExSetAsData;
TCanExSetDataPtr CanExSetDataPtr = DefCanExSetDataPtr;
TCanExSetDataBlockCB CanExSetDataBlockCB = DefCanExSetDataBlockCB;

TCanExGetInfoList CanExGetInfoList = DefCanExGetInfoList;
TCanExGetInfoListPerform CanExGetInfoListPerform = DefCanExGetInfoListPerform;
TCanExGetInfoListGet CanExGetInfoListGet = DefCanExGetInfoListGet;

TMhsCanGetApiHandle MhsCanGetApiHandle = DefMhsCanGetApiHandle;

#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu
TCanExIoOpen CanExIoOpen = DefCanExIoOpen;
TCanExIoClose CanExIoClose = DefCanExIoClose;
TCanExIoWrite CanExIoWrite = DefCanExIoWrite;
TCanExIoRead CanExIoRead = DefCanExIoRead;
#endif


#endif

#ifdef LINUX_HAVE_GET_API_DRIVER_PATH
static char *DriverFileName = NULL;
#endif

#ifdef __WIN32__
static HINSTANCE DriverHandle = 0;               // Handle to DLL
static char *DriverFileName = NULL;

  #if _MSC_VER >= 1500
    #define INTERLOCK_TYPE_CONV
  #else
    #define INTERLOCK_TYPE_CONV (volatile LPLONG)
  #endif
#else
static void *DriverHandle = NULL;
#endif


#ifdef DRV_REF_LOCKING

  #ifdef __WIN32__
    // ****** Windows
static volatile LONG TCanDrvRefCounter = 0;

    #define CAN_DRV_REF() (void)InterlockedIncrement(INTERLOCK_TYPE_CONV&TCanDrvRefCounter)
    #define CAN_DRV_UNREF() (void)InterlockedDecrement(INTERLOCK_TYPE_CONV&TCanDrvRefCounter)
  #else
    // ****** Linux
static volatile uint32_t TCanDrvRefCounter = 0;
    #define CAN_DRV_REF() (void) __sync_fetch_and_add(&TCanDrvRefCounter, 1)
    #define CAN_DRV_UNREF() (void) __sync_fetch_and_sub(&TCanDrvRefCounter, 1)
  #endif
#else
  #define CAN_DRV_REF()
  #define CAN_DRV_UNREF()
#endif


#ifdef __WIN32__
  // ****** Windows
  #define mhs_sleep(x) Sleep(x)
  #define MhsGetProcAddress(h, s) (void *)GetProcAddress((h), (LPCSTR)(s))
#else
  // ****** Linux
  #define mhs_sleep(x) usleep((x) * 1000)
  #define MhsGetProcAddress(h, s) dlsym((h), (s))
#endif


#if defined(__WIN32__) || defined(LINUX_HAVE_GET_API_DRIVER_PATH)

#ifdef __GNUC__
  #define SafeSprintf snprintf
#endif

#ifdef __WIN32__
  #ifndef __GNUC__
    #if defined(_MSC_VER) && _MSC_VER >= 1500
      #define SafeSprintf(dst, dst_max, format, ...) _snprintf_s(dst, dst_max, _TRUNCATE, format, __VA_ARGS__)
    #else
      #define SafeSprintf _snprintf
    #endif
  #endif
#endif

#define MhsSafeFree(d) do { \
  if ((d)) \
    { \
    free((d)); \
    (d) = NULL; \
    } \
  } while(0)

static char *GetApiDriverWithPath(const char *driver_file);
#endif


/***************************************************************/
/*  Treiber DLL laden                                          */
/***************************************************************/
int32_t LoadDriver(const char *file_name)
{
int32_t error;

error = 0;
UnloadDriver();
#ifdef __WIN32__
if (!(DriverFileName = GetApiDriverWithPath(file_name)))
  {
#ifdef MHS_DRV_DEBUG_OUTPUT
printf("Fehler bei GetApiDriverWithPath\n\r");
#endif
  return(-100);
  }
DriverHandle = LoadLibraryA(DriverFileName);
  #ifdef MHS_DRV_DEBUG_OUTPUT
if (DriverHandle)
  printf("API driver file \"%s\" loaded.\n\r", DriverFileName);
else
  printf("Error load: %s\n\r", DriverFileName);
  #endif
#else
  #ifdef LINUX_HAVE_GET_API_DRIVER_PATH
if (!(DriverFileName = GetApiDriverWithPath(file_name)))
  {
#ifdef MHS_DRV_DEBUG_OUTPUT
printf("Fehler bei GetApiDriverWithPath\n\r");
#endif
  return(-100);
  }
DriverHandle = dlopen(DriverFileName, RTLD_LAZY);
  #else
DriverHandle = dlopen(file_name, RTLD_LAZY);
  #endif
  #ifdef MHS_DRV_DEBUG_OUTPUT
if (DriverHandle)
  printf("API driver file \"%s\" loaded.\n\r", DriverFileName);
else
  printf("Error load: %s\n\r, ", dlerror());
  #endif
#endif
if (DriverHandle)
  {
#ifdef CAN_API_TRUE_FUNC
  if  (!(PCanInitDriver = (TCanInitDriver)MhsGetProcAddress(DriverHandle, "CanInitDriver")))
    error = -1;
  else if (!(PCanDownDriver = (TCanDownDriver)MhsGetProcAddress(DriverHandle, "CanDownDriver")))
    error = -2;
  else if (!(PCanSetOptions = (TCanSetOptions)MhsGetProcAddress(DriverHandle, "CanSetOptions")))
    error = -3;
  else if (!(PCanDeviceOpen = (TCanDeviceOpen)MhsGetProcAddress(DriverHandle, "CanDeviceOpen")))
    error = -4;
  else if (!(PCanDeviceClose = (TCanDeviceClose)MhsGetProcAddress(DriverHandle, "CanDeviceClose")))
    error = -5;
  else if (!(PCanSetMode = (TCanSetMode)MhsGetProcAddress(DriverHandle, "CanSetMode")))
    error = -6;
  else if (!(PCanTransmit = (TCanTransmit)MhsGetProcAddress(DriverHandle, "CanTransmit")))
    error = -7;
  else if (!(PCanTransmitClear = (TCanTransmitClear)MhsGetProcAddress(DriverHandle, "CanTransmitClear")))
    error = -8;
  else if (!(PCanTransmitGetCount = (TCanTransmitGetCount)MhsGetProcAddress(DriverHandle, "CanTransmitGetCount")))
    error = -9;
  else if (!(PCanTransmitSet = (TCanTransmitSet)MhsGetProcAddress(DriverHandle, "CanTransmitSet")))
    error = -10;
  else if (!(PCanReceive = (TCanReceive)MhsGetProcAddress(DriverHandle, "CanReceive")))
    error = -11;
  else if (!(PCanReceiveClear = (TCanReceiveClear)MhsGetProcAddress(DriverHandle, "CanReceiveClear")))
    error = -12;
  else if (!(PCanReceiveGetCount = (TCanReceiveGetCount)MhsGetProcAddress(DriverHandle, "CanReceiveGetCount")))
    error = -13;
  else if (!(PCanSetSpeed = (TCanSetSpeed)MhsGetProcAddress(DriverHandle, "CanSetSpeed")))
    error = -14;
  else if (!(PCanSetSpeedUser = (TCanSetSpeedUser)MhsGetProcAddress(DriverHandle, "CanSetSpeedUser")))
    error = -15;
  else if (!(PCanDrvInfo = (TCanDrvInfo)MhsGetProcAddress(DriverHandle, "CanDrvInfo")))
    error = -16;
  else if (!(PCanDrvHwInfo = (TCanDrvHwInfo)MhsGetProcAddress(DriverHandle, "CanDrvHwInfo")))
    error = -17;
  else if (!(PCanSetFilter = (TCanSetFilter)MhsGetProcAddress(DriverHandle, "CanSetFilter")))
    error = -18;
  else if (!(PCanGetDeviceStatus = (TCanGetDeviceStatus)MhsGetProcAddress(DriverHandle, "CanGetDeviceStatus")))
    error = -19;
  else if (!(PCanSetPnPEventCallback = (TCanSetPnPEventCallback)MhsGetProcAddress(DriverHandle, "CanSetPnPEventCallback")))
    error = -20;
  else if (!(PCanSetStatusEventCallback = (TCanSetStatusEventCallback)MhsGetProcAddress(DriverHandle, "CanSetStatusEventCallback")))
    error = -21;
  else if (!(PCanSetRxEventCallback = (TCanSetRxEventCallback)MhsGetProcAddress(DriverHandle, "CanSetRxEventCallback")))
    error = -22;
  else if (!(PCanSetEvents = (TCanSetEvents)MhsGetProcAddress(DriverHandle, "CanSetEvents")))
    error = -23;
  else if (!(PCanEventStatus = (TCanEventStatus)MhsGetProcAddress(DriverHandle, "CanEventStatus")))
    error = -24;
  // EX-Funktionen
  else if (!(PCanExGetDeviceCount = (TCanExGetDeviceCount)MhsGetProcAddress(DriverHandle, "CanExGetDeviceCount")))
    error = -25;
  else if (!(PCanExGetDeviceList = (TCanExGetDeviceList)MhsGetProcAddress(DriverHandle, "CanExGetDeviceList")))
    error = -26;
  else if (!(PCanExGetDeviceListPerform = (TCanExGetDeviceListPerform)MhsGetProcAddress(DriverHandle, "CanExGetDeviceListPerform")))
    error = -27;
    else if (!(PCanExGetDeviceListGet = (TCanExGetDeviceListGet)MhsGetProcAddress(DriverHandle, "CanExGetDeviceListGet")))
    error = -28;
  else if (!(PCanExGetDeviceInfo = (TCanExGetDeviceInfo)MhsGetProcAddress(DriverHandle, "CanExGetDeviceInfo")))
    error = -29;
  else if (!(PCanExGetDeviceInfoPerform = (TCanExGetDeviceInfoPerform)MhsGetProcAddress(DriverHandle, "CanExGetDeviceInfoPerform")))
    error = -30;
  else if (!(PCanExGetDeviceInfoGet = (TCanExGetDeviceInfoGet)MhsGetProcAddress(DriverHandle, "CanExGetDeviceInfoGet")))
    error = -31;
  else if (!(PCanExDataFree = (TCanExDataFree)MhsGetProcAddress(DriverHandle, "CanExDataFree")))
    error = -32;
  else if (!(PCanExCreateDevice = (TCanExCreateDevice)MhsGetProcAddress(DriverHandle, "CanExCreateDevice")))
    error = -33;
  else if (!(PCanExDestroyDevice = (TCanExDestroyDevice)MhsGetProcAddress(DriverHandle, "CanExDestroyDevice")))
    error = -34;
  else if (!(PCanExCreateFifo = (TCanExCreateFifo)MhsGetProcAddress(DriverHandle, "CanExCreateFifo")))
    error = -35;
  else if (!(PCanExBindFifo = (TCanExBindFifo)MhsGetProcAddress(DriverHandle, "CanExBindFifo")))
    error = -36;
  else if (!(PCanExCreateEvent = (TCanExCreateEvent)MhsGetProcAddress(DriverHandle, "CanExCreateEvent")))
    error = -37;
  else if (!(PCanExSetObjEvent = (TCanExSetObjEvent)MhsGetProcAddress(DriverHandle, "CanExSetObjEvent")))
    error = -38;
  else if (!(PCanExSetEvent = (TCanExSetEvent)MhsGetProcAddress(DriverHandle, "CanExSetEvent")))
    error = -39;
  else if (!(PCanExResetEvent = (TCanExResetEvent)MhsGetProcAddress(DriverHandle, "CanExResetEvent")))
    error = -40;
  else if (!(PCanExWaitForEvent = (TCanExWaitForEvent)MhsGetProcAddress(DriverHandle, "CanExWaitForEvent")))
    error = -41;
  else if (!(PCanExInitDriver = (TCanExInitDriver)MhsGetProcAddress(DriverHandle, "CanExInitDriver")))
    error = -42;
  else if (!(PCanExSetOptions = (TCanExSetOptions)MhsGetProcAddress(DriverHandle, "CanExSetOptions")))
    error = -43;
  else if (!(PCanExSetAsByte = (TCanExSetAsByte)MhsGetProcAddress(DriverHandle, "CanExSetAsByte")))
    error = -44;
  else if (!(PCanExSetAsWord = (TCanExSetAsWord)MhsGetProcAddress(DriverHandle, "CanExSetAsWord")))
    error = -45;
  else if (!(PCanExSetAsLong = (TCanExSetAsLong)MhsGetProcAddress(DriverHandle, "CanExSetAsLong")))
    error = -46;
  else if (!(PCanExSetAsUByte = (TCanExSetAsUByte)MhsGetProcAddress(DriverHandle, "CanExSetAsUByte")))
    error = -47;
  else if (!(PCanExSetAsUWord = (TCanExSetAsUWord)MhsGetProcAddress(DriverHandle, "CanExSetAsUWord")))
    error = -48;
  else if (!(PCanExSetAsULong = (TCanExSetAsULong)MhsGetProcAddress(DriverHandle, "CanExSetAsULong")))
    error = -49;
  else if (!(PCanExSetAsString = (TCanExSetAsString)MhsGetProcAddress(DriverHandle, "CanExSetAsString")))
    error = -50;
  else if (!(PCanExGetAsByte = (TCanExGetAsByte)MhsGetProcAddress(DriverHandle, "CanExGetAsByte")))
    error = -51;
  else if (!(PCanExGetAsWord = (TCanExGetAsWord)MhsGetProcAddress(DriverHandle, "CanExGetAsWord")))
    error = -52;
  else if (!(PCanExGetAsLong = (TCanExGetAsLong)MhsGetProcAddress(DriverHandle, "CanExGetAsLong")))
    error = -53;
  else if (!(PCanExGetAsUByte = (TCanExGetAsUByte)MhsGetProcAddress(DriverHandle, "CanExGetAsUByte")))
    error = -54;
  else if (!(PCanExGetAsUWord = (TCanExGetAsUWord)MhsGetProcAddress(DriverHandle, "CanExGetAsUWord")))
    error = -55;
  else if (!(PCanExGetAsULong = (TCanExGetAsULong)MhsGetProcAddress(DriverHandle, "CanExGetAsULong")))
    error = -56;
  else if (!(PCanExGetAsString = (TCanExGetAsString)MhsGetProcAddress(DriverHandle, "CanExGetAsString")))
    error = -57;
  else if (!(PCanExGetAsStringCopy = (TCanExGetAsStringCopy)MhsGetProcAddress(DriverHandle, "CanExGetAsStringCopy")))
    error = -58;
  else if (!(PCanExSetEventAll = (TCanExSetEventAll)MhsGetProcAddress(DriverHandle, "CanExSetEventAll")))
    error = -59;
  // **** CAN-FD
#ifdef STRICT_CAN_FD_SUPPORT
  else if (!(PCanFdTransmit = (TCanFdTransmit)MhsGetProcAddress(DriverHandle, "CanFdTransmit")))
    error = -60;
  else if (!(PCanFdReceive = (TCanFdReceive)MhsGetProcAddress(DriverHandle, "CanFdReceive")))
    error = -61;
#else
  PCanFdTransmit = (TCanFdTransmit)MhsGetProcAddress(DriverHandle, "CanFdTransmit");
  PCanFdReceive = (TCanFdReceive)MhsGetProcAddress(DriverHandle, "CanFdReceive");
#endif
// ****** <*> neu
#ifdef STRICT_ECU_FLASH_SUPPORT
  if (!(PCanExSetAsData = (TCanExSetAsData)MhsGetProcAddress(DriverHandle, "CanExSetAsData")))
    error = -62;
  if (!(PCanExSetDataPtr = (TCanExSetDataPtr)MhsGetProcAddress(DriverHandle, "CanExSetDataPtr")))
    error = -63;
  if (!(PCanExSetDataBlockCB = (TCanExSetDataBlockCB)MhsGetProcAddress(DriverHandle, "CanExSetDataBlockCB")))
    error = -64;
#else
  PCanExSetAsData = (TCanExSetAsData)MhsGetProcAddress(DriverHandle, "CanExSetAsData");
  PCanExSetDataPtr = (TCanExSetDataPtr)MhsGetProcAddress(DriverHandle, "CanExSetDataPtr");
  PCanExSetDataBlockCB = (TCanExSetDataBlockCB)MhsGetProcAddress(DriverHandle, "CanExSetDataBlockCB");
#endif
// ****** <*> neu2
  PCanExGetInfoList = (TCanExGetInfoList)MhsGetProcAddress(DriverHandle, "CanExGetInfoList");
  PCanExGetInfoListPerform = (TCanExGetInfoListPerform)MhsGetProcAddress(DriverHandle, "CanExGetInfoListPerform");
  PCanExGetInfoListGet = (TCanExGetInfoListGet)MhsGetProcAddress(DriverHandle, "CanExGetInfoListGet");
  PMhsCanGetApiHandle = (TMhsCanGetApiHandle)MhsGetProcAddress(DriverHandle,  "MhsCanGetApiHandle");
#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu
  PCanExIoOpen = (TCanExIoOpen)MhsGetProcAddress(DriverHandle, "CanExIoOpen");
  PCanExIoClose = (TCanExIoClose)MhsGetProcAddress(DriverHandle, "CanExIoClose");
  PCanExIoWrite = (TCanExIoWrite)MhsGetProcAddress(DriverHandle, "CanExIoWrite");
  PCanExIoRead = (TCanExIoRead)MhsGetProcAddress(DriverHandle, "CanExIoRead");
#endif
#else
  if  (!(CanInitDriver = (TCanInitDriver)MhsGetProcAddress(DriverHandle, "CanInitDriver")))
    error = -1;
  else if (!(CanDownDriver = (TCanDownDriver)MhsGetProcAddress(DriverHandle, "CanDownDriver")))
    error = -2;
  else if (!(CanSetOptions = (TCanSetOptions)MhsGetProcAddress(DriverHandle, "CanSetOptions")))
    error = -3;
  else if (!(CanDeviceOpen = (TCanDeviceOpen)MhsGetProcAddress(DriverHandle, "CanDeviceOpen")))
    error = -4;
  else if (!(CanDeviceClose = (TCanDeviceClose)MhsGetProcAddress(DriverHandle, "CanDeviceClose")))
    error = -5;
  else if (!(CanSetMode = (TCanSetMode)MhsGetProcAddress(DriverHandle, "CanSetMode")))
    error = -7;
  else if (!(CanTransmit = (TCanTransmit)MhsGetProcAddress(DriverHandle, "CanTransmit")))
    error = -10;
  else if (!(CanTransmitClear = (TCanTransmitClear)MhsGetProcAddress(DriverHandle, "CanTransmitClear")))
    error = -11;
  else if (!(CanTransmitGetCount = (TCanTransmitGetCount)MhsGetProcAddress(DriverHandle, "CanTransmitGetCount")))
    error = -12;
  else if (!(CanTransmitSet = (TCanTransmitSet)MhsGetProcAddress(DriverHandle, "CanTransmitSet")))
    error = -13;
  else if (!(CanReceive = (TCanReceive)MhsGetProcAddress(DriverHandle, "CanReceive")))
    error = -14;
  else if (!(CanReceiveClear = (TCanReceiveClear)MhsGetProcAddress(DriverHandle, "CanReceiveClear")))
    error = -15;
  else if (!(CanReceiveGetCount = (TCanReceiveGetCount)MhsGetProcAddress(DriverHandle, "CanReceiveGetCount")))
    error = -16;
  else if (!(CanSetSpeed = (TCanSetSpeed)MhsGetProcAddress(DriverHandle, "CanSetSpeed")))
    error = -17;
  else if (!(CanSetSpeedUser = (TCanSetSpeedUser)MhsGetProcAddress(DriverHandle, "CanSetSpeedUser")))
    error = -18;
  else if (!(CanDrvInfo = (TCanDrvInfo)MhsGetProcAddress(DriverHandle, "CanDrvInfo")))
    error = -19;
  else if (!(CanDrvHwInfo = (TCanDrvHwInfo)MhsGetProcAddress(DriverHandle, "CanDrvHwInfo")))
    error = -20;
  else if (!(CanSetFilter = (TCanSetFilter)MhsGetProcAddress(DriverHandle, "CanSetFilter")))
    error = -21;
  else if (!(CanGetDeviceStatus = (TCanGetDeviceStatus)MhsGetProcAddress(DriverHandle, "CanGetDeviceStatus")))
    error = -22;
  else if (!(CanSetPnPEventCallback = (TCanSetPnPEventCallback)MhsGetProcAddress(DriverHandle, "CanSetPnPEventCallback")))
    error = -23;
  else if (!(CanSetStatusEventCallback = (TCanSetStatusEventCallback)MhsGetProcAddress(DriverHandle, "CanSetStatusEventCallback")))
    error = -24;
  else if (!(CanSetRxEventCallback = (TCanSetRxEventCallback)MhsGetProcAddress(DriverHandle, "CanSetRxEventCallback")))
    error = -25;
  else if (!(CanSetEvents = (TCanSetEvents)MhsGetProcAddress(DriverHandle, "CanSetEvents")))
    error = -26;
  else if (!(CanEventStatus = (TCanEventStatus)MhsGetProcAddress(DriverHandle, "CanEventStatus")))
    error = -27;
  // EX-Funktionen
  else if (!(CanExGetDeviceCount = (TCanExGetDeviceCount)MhsGetProcAddress(DriverHandle, "CanExGetDeviceCount")))
    error = -28;
  else if (!(CanExGetDeviceList = (TCanExGetDeviceList)MhsGetProcAddress(DriverHandle, "CanExGetDeviceList")))
    error = -29;
  else if (!(CanExGetDeviceListPerform = (TCanExGetDeviceListPerform)MhsGetProcAddress(DriverHandle, "CanExGetDeviceListPerform")))
    error = -57;
  else if (!(CanExGetDeviceListGet = (TCanExGetDeviceListGet)MhsGetProcAddress(DriverHandle, "CanExGetDeviceListGet")))
    error = -58;
  else if (!(CanExGetDeviceInfo = (TCanExGetDeviceInfo)MhsGetProcAddress(DriverHandle, "CanExGetDeviceInfo")))
    error = -30;
  else if (!(CanExGetDeviceInfoPerform = (TCanExGetDeviceInfoPerform)MhsGetProcAddress(DriverHandle, "CanExGetDeviceInfoPerform")))
    error = -59;
  else if (!(CanExGetDeviceInfoGet = (TCanExGetDeviceInfoGet)MhsGetProcAddress(DriverHandle, "CanExGetDeviceInfoGet")))
    error = -60;
  else if (!(CanExDataFree = (TCanExDataFree)MhsGetProcAddress(DriverHandle, "CanExDataFree")))
    error = -31;
  else if (!(CanExCreateDevice = (TCanExCreateDevice)MhsGetProcAddress(DriverHandle, "CanExCreateDevice")))
    error = -32;
  else if (!(CanExDestroyDevice = (TCanExDestroyDevice)MhsGetProcAddress(DriverHandle, "CanExDestroyDevice")))
    error = -33;
  else if (!(CanExCreateFifo = (TCanExCreateFifo)MhsGetProcAddress(DriverHandle, "CanExCreateFifo")))
    error = -34;
  else if (!(CanExBindFifo = (TCanExBindFifo)MhsGetProcAddress(DriverHandle, "CanExBindFifo")))
    error = -35;
  else if (!(CanExCreateEvent = (TCanExCreateEvent)MhsGetProcAddress(DriverHandle, "CanExCreateEvent")))
    error = -36;
  else if (!(CanExSetObjEvent = (TCanExSetObjEvent)MhsGetProcAddress(DriverHandle, "CanExSetObjEvent")))
    error = -37;
  else if (!(CanExSetEvent = (TCanExSetEvent)MhsGetProcAddress(DriverHandle, "CanExSetEvent")))
    error = -38;
  else if (!(CanExResetEvent = (TCanExResetEvent)MhsGetProcAddress(DriverHandle, "CanExResetEvent")))
    error = -39;
  else if (!(CanExWaitForEvent = (TCanExWaitForEvent)MhsGetProcAddress(DriverHandle, "CanExWaitForEvent")))
    error = -40;
  else if (!(CanExInitDriver = (TCanExInitDriver)MhsGetProcAddress(DriverHandle, "CanExInitDriver")))
    error = -41;
  else if (!(CanExSetOptions = (TCanExSetOptions)MhsGetProcAddress(DriverHandle, "CanExSetOptions")))
    error = -42;
  else if (!(CanExSetAsByte = (TCanExSetAsByte)MhsGetProcAddress(DriverHandle, "CanExSetAsByte")))
    error = -43;
  else if (!(CanExSetAsWord = (TCanExSetAsWord)MhsGetProcAddress(DriverHandle, "CanExSetAsWord")))
    error = -44;
  else if (!(CanExSetAsLong = (TCanExSetAsLong)MhsGetProcAddress(DriverHandle, "CanExSetAsLong")))
    error = -45;
  else if (!(CanExSetAsUByte = (TCanExSetAsUByte)MhsGetProcAddress(DriverHandle, "CanExSetAsUByte")))
    error = -46;
  else if (!(CanExSetAsUWord = (TCanExSetAsUWord)MhsGetProcAddress(DriverHandle, "CanExSetAsUWord")))
    error = -47;
  else if (!(CanExSetAsULong = (TCanExSetAsULong)MhsGetProcAddress(DriverHandle, "CanExSetAsULong")))
    error = -48;
  else if (!(CanExSetAsString = (TCanExSetAsString)MhsGetProcAddress(DriverHandle, "CanExSetAsString")))
    error = -49;
  else if (!(CanExGetAsByte = (TCanExGetAsByte)MhsGetProcAddress(DriverHandle, "CanExGetAsByte")))
    error = -50;
  else if (!(CanExGetAsWord = (TCanExGetAsWord)MhsGetProcAddress(DriverHandle, "CanExGetAsWord")))
    error = -51;
  else if (!(CanExGetAsLong = (TCanExGetAsLong)MhsGetProcAddress(DriverHandle, "CanExGetAsLong")))
    error = -52;
  else if (!(CanExGetAsUByte = (TCanExGetAsUByte)MhsGetProcAddress(DriverHandle, "CanExGetAsUByte")))
    error = -53;
  else if (!(CanExGetAsUWord = (TCanExGetAsUWord)MhsGetProcAddress(DriverHandle, "CanExGetAsUWord")))
    error = -54;
  else if (!(CanExGetAsULong = (TCanExGetAsULong)MhsGetProcAddress(DriverHandle, "CanExGetAsULong")))
    error = -55;
  else if (!(CanExGetAsString = (TCanExGetAsString)MhsGetProcAddress(DriverHandle, "CanExGetAsString")))
    error = -56;
  else if (!(CanExGetAsStringCopy = (TCanExGetAsStringCopy)MhsGetProcAddress(DriverHandle, "CanExGetAsStringCopy")))
    error = -61;
  else if (!(CanExSetEventAll = (TCanExSetEventAll)MhsGetProcAddress(DriverHandle, "CanExSetEventAll")))
    error = -62;
#ifdef STRICT_CAN_FD_SUPPORT
  else if (!(CanFdTransmit = (TCanFdTransmit)MhsGetProcAddress(DriverHandle, "CanFdTransmit")))
    error = -63;
  else if (!(CanFdReceive = (TCanFdReceive)MhsGetProcAddress(DriverHandle, "CanFdReceive")))
    error = -64;
#else
  if (!(CanFdTransmit = (TCanFdTransmit)MhsGetProcAddress(DriverHandle, "CanFdTransmit")))
    CanFdTransmit = DefCanFdTransmit;
  if (!(CanFdReceive = (TCanFdReceive)MhsGetProcAddress(DriverHandle, "CanFdReceive")))
    CanFdReceive = DefCanFdReceive;
#endif
  // **** <*> neu
#ifdef STRICT_ECU_FLASH_SUPPORT
  if (!(CanExSetAsData = (TCanExSetAsData)MhsGetProcAddress(DriverHandle, "CanExSetAsData")))
    error = -65;
  if (!(CanExSetDataPtr = (TCanExSetDataPtr)MhsGetProcAddress(DriverHandle, "CanExSetDataPtr")))
    error = -66;
  if (!(CanExSetDataBlockCB = (TCanExSetDataBlockCB)MhsGetProcAddress(DriverHandle, "CanExSetDataBlockCB")))
    error = -64;
#else
  if (!(CanExSetAsData = (TCanExSetAsData)MhsGetProcAddress(DriverHandle, "CanExSetAsData")))
    CanExSetAsData = DefCanExSetAsData;
  if (!(CanExSetDataPtr = (TCanExSetDataPtr)MhsGetProcAddress(DriverHandle, "CanExSetDataPtr")))
    CanExSetDataPtr = DefCanExSetDataPtr;
  if (!(CanExSetDataBlockCB = (TCanExSetDataBlockCB)MhsGetProcAddress(DriverHandle, "CanExSetDataBlockCB")))
    CanExSetDataBlockCB = DefCanExSetDataBlockCB;
#endif
  if (!(CanExGetInfoList = (TCanExGetInfoList)MhsGetProcAddress(DriverHandle, "CanExGetInfoList")))
    CanExGetInfoList = DefCanExGetInfoList;
  if (!(CanExGetInfoListPerform = (TCanExGetInfoListPerform)MhsGetProcAddress(DriverHandle, "CanExGetInfoListPerform")))
    CanExGetInfoListPerform = DefCanExGetInfoListPerform;
  if (!(CanExGetInfoListGet = (TCanExGetInfoListGet)MhsGetProcAddress(DriverHandle, "CanExGetInfoListGet")))
    CanExGetInfoListGet = DefCanExGetInfoListGet;
  if (!(MhsCanGetApiHandle = (TMhsCanGetApiHandle)MhsGetProcAddress(DriverHandle, "MhsCanGetApiHandle")))
    MhsCanGetApiHandle = DefMhsCanGetApiHandle;
#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu
  if (!(CanExIoOpen = (TCanExIoOpen)MhsGetProcAddress(DriverHandle, "CanExIoOpen")))
    CanExIoOpen = DefCanExIoOpen;
  if (!(CanExIoClose = (TCanExIoClose)MhsGetProcAddress(DriverHandle, "CanExIoClose")))
    CanExIoClose = DefCanExIoClose;
  if (!(CanExIoWrite = (TCanExIoWrite)MhsGetProcAddress(DriverHandle, "CanExIoWrite")))
    CanExIoWrite = DefCanExIoWrite;
  if (!(CanExIoRead = (TCanExIoRead)MhsGetProcAddress(DriverHandle, "CanExIoRead")))
    CanExIoRead = DefCanExIoRead;
#endif
#endif
  }
else
  error = -100;
if (error)
  UnloadDriver();
return(error);
}


/***************************************************************/
/*  Treiber DLL entladen                                       */
/***************************************************************/
void UnloadDriver(void)
{
#ifdef __WIN32__
HINSTANCE driver_handle;
#else
void *driver_handle;
#endif

driver_handle = DriverHandle;
DriverHandle = NULL;
#ifdef __WIN32__
MhsSafeFree(DriverFileName);
#endif
if (driver_handle)
  CanDownDriver();
#ifdef CAN_API_TRUE_FUNC
PCanInitDriver = NULL;
PCanDownDriver = NULL;
PCanSetOptions = NULL;
PCanDeviceOpen = NULL;
PCanDeviceClose = NULL;
PCanSetMode = NULL;
PCanTransmit = NULL;
PCanTransmitClear = NULL;
PCanTransmitGetCount = NULL;
PCanTransmitSet = NULL;
PCanReceive = NULL;
PCanReceiveClear = NULL;
PCanReceiveGetCount = NULL;
PCanSetSpeed = NULL;
PCanSetSpeedUser = NULL;
PCanDrvInfo = NULL;
PCanDrvHwInfo = NULL;
PCanSetFilter = NULL;
PCanGetDeviceStatus = NULL;
PCanSetPnPEventCallback = NULL;
PCanSetStatusEventCallback = NULL;
PCanSetRxEventCallback = NULL;
PCanSetEvents = NULL;
PCanEventStatus = NULL;
// EX-Funktionen
PCanExGetDeviceCount = NULL;
PCanExGetDeviceList = NULL;
PCanExGetDeviceListPerform = NULL;
PCanExGetDeviceListGet = NULL;
PCanExGetDeviceInfo = NULL;
PCanExGetDeviceInfoPerform = NULL;
PCanExGetDeviceInfoGet = NULL;
PCanExDataFree = NULL;
PCanExCreateDevice = NULL;
PCanExDestroyDevice = NULL;
PCanExCreateFifo = NULL;
PCanExBindFifo = NULL;
PCanExCreateEvent = NULL;
PCanExSetObjEvent = NULL;
PCanExSetEvent = NULL;
PCanExSetEventAll = NULL;
PCanExResetEvent = NULL;
PCanExWaitForEvent = NULL;
PCanExInitDriver = NULL;
PCanExSetOptions = NULL;
PCanExSetAsByte = NULL;
PCanExSetAsWord = NULL;
PCanExSetAsLong = NULL;
PCanExSetAsUByte = NULL;
PCanExSetAsUWord = NULL;
PCanExSetAsULong = NULL;
PCanExSetAsString = NULL;
PCanExGetAsByte = NULL;
PCanExGetAsWord = NULL;
PCanExGetAsLong = NULL;
PCanExGetAsUByte = NULL;
PCanExGetAsUWord = NULL;
PCanExGetAsULong = NULL;
PCanExGetAsString = NULL;
PCanExGetAsStringCopy = NULL;
// **** CAN-FD
PCanFdTransmit = NULL;
PCanFdReceive = NULL;
// <*> neu
PCanExSetAsData = NULL;
PCanExSetDataPtr = NULL;
PCanExSetDataBlockCB = NULL;
PCanExGetInfoList = NULL;
PCanExGetInfoListPerform = NULL;
PCanExGetInfoListGet = NULL;
PMhsCanGetApiHandle = NULL;
#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu
PCanExIoOpen = NULL;
PCanExIoClose = NULL;
PCanExIoWrite = NULL;
PCanExIoRead = NULL;
#endif
#else
CanInitDriver = DefCanInitDriver;
CanDownDriver = DefCanDownDriver;
CanSetOptions = DefCanSetOptions;
CanDeviceOpen = DefCanDeviceOpen;
CanDeviceClose = DefCanDeviceClose;
CanSetMode = DefCanSetMode;
CanTransmit = DefCanTransmit;
CanTransmitClear = DefCanTransmitClear;
CanTransmitGetCount = DefCanTransmitGetCount;
CanTransmitSet = DefCanTransmitSet;
CanReceive = DefCanReceive;
CanReceiveClear = DefCanReceiveClear;
CanReceiveGetCount = DefCanReceiveGetCount;
CanSetSpeed = DefCanSetSpeed;
CanSetSpeedUser = DefCanSetSpeedUser;
CanDrvInfo = DefCanDrvInfo;
CanDrvHwInfo = DefCanDrvHwInfo;
CanSetFilter = DefCanSetFilter;
CanGetDeviceStatus = DefCanGetDeviceStatus;
CanSetPnPEventCallback = DefCanSetPnPEventCallback;
CanSetStatusEventCallback = DefCanSetStatusEventCallback;
CanSetRxEventCallback = DefCanSetRxEventCallback;
CanSetEvents = DefCanSetEvents;
CanEventStatus = DefCanEventStatus;
// EX-Funktionen
CanExGetDeviceCount = DefCanExGetDeviceCount;
CanExGetDeviceList = DefCanExGetDeviceList;
CanExGetDeviceListPerform = DefCanExGetDeviceListPerform;
CanExGetDeviceListGet = DefCanExGetDeviceListGet;
CanExGetDeviceInfo = DefCanExGetDeviceInfo;
CanExGetDeviceInfoPerform = DefCanExGetDeviceInfoPerform;
CanExGetDeviceInfoGet = DefCanExGetDeviceInfoGet;
CanExDataFree = DefCanExDataFree;
CanExCreateDevice = DefCanExCreateDevice;
CanExDestroyDevice = DefCanExDestroyDevice;
CanExCreateFifo = DefCanExCreateFifo;
CanExBindFifo = DefCanExBindFifo;
CanExCreateEvent = DefCanExCreateEvent;
CanExSetObjEvent = DefCanExSetObjEvent;
CanExSetEvent = DefCanExSetEvent;
CanExSetEventAll = DefCanExSetEventAll;
CanExResetEvent = DefCanExResetEvent;
CanExWaitForEvent = DefCanExWaitForEvent;
CanExInitDriver = DefCanExInitDriver;
CanExSetOptions = DefCanExSetOptions;
CanExSetAsByte = DefCanExSetAsByte;
CanExSetAsWord = DefCanExSetAsWord;
CanExSetAsLong = DefCanExSetAsLong;
CanExSetAsUByte = DefCanExSetAsUByte;
CanExSetAsUWord = DefCanExSetAsUWord;
CanExSetAsULong = DefCanExSetAsULong;
CanExSetAsString = DefCanExSetAsString;
CanExGetAsByte = DefCanExGetAsByte;
CanExGetAsWord = DefCanExGetAsWord;
CanExGetAsLong = DefCanExGetAsLong;
CanExGetAsUByte = DefCanExGetAsUByte;
CanExGetAsUWord = DefCanExGetAsUWord;
CanExGetAsULong = DefCanExGetAsULong;
CanExGetAsString = DefCanExGetAsString;
CanExGetAsStringCopy = DefCanExGetAsStringCopy;
CanFdTransmit = DefCanFdTransmit;
CanFdReceive = DefCanFdReceive;
// <*> neu
CanExSetAsData = DefCanExSetAsData;
CanExSetDataPtr = DefCanExSetDataPtr;
CanExSetDataBlockCB = DefCanExSetDataBlockCB;
CanExGetInfoList = DefCanExGetInfoList;
CanExGetInfoListPerform = DefCanExGetInfoListPerform;
CanExGetInfoListGet = DefCanExGetInfoListGet;
MhsCanGetApiHandle = DefMhsCanGetApiHandle;
#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu
CanExIoOpen = DefCanExIoOpen;
CanExIoClose = DefCanExIoClose;
CanExIoWrite = DefCanExIoWrite;
CanExIoRead = DefCanExIoRead;
#endif

#endif
if (driver_handle)
  {
#ifdef __WIN32__
  FreeLibrary(driver_handle);
#else
  dlclose(driver_handle);
#endif
  }
}


#ifdef CAN_API_TRUE_FUNC

/***************************************************************/
/*  API Funktionen                                             */
/***************************************************************/
int32_t CanInitDriver(char *options)
{
if (PCanInitDriver)
  return((PCanInitDriver)(options));
else
  return(ERR_NO_CAN_DRIVER_LOAD);
}


void CanDownDriver(void)
{
#ifdef DRV_REF_LOCKING
uint32_t cnt, i;
#endif

if (PCanDownDriver)
  {
  // **** Alle Events sperren
  if (PCanSetEvents)      // CanSetEvents(0);
    (PCanSetEvents)(0);
  if (PCanExSetEventAll)  // CanExSetEventAll(MHS_TERMINATE);
    (PCanExSetEventAll)(MHS_TERMINATE);
#ifdef DRV_REF_LOCKING
  for (i = 5; i; i--)
    {
    cnt = 100;
    while (TCanDrvRefCounter)
      {
      mhs_sleep(25);
      if (!--cnt)
        break;
      }
    mhs_sleep(1);
    if (!TCanDrvRefCounter)
      break;
    }
  TCanDrvRefCounter = 0;
#endif
  (PCanDownDriver)();
  }
}


int32_t CanSetOptions(char *options)
{
int32_t err;

if ((DriverHandle) && (PCanSetOptions))
  {
  CAN_DRV_REF();
  err = (PCanSetOptions)(options);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanDeviceOpen(uint32_t  index, char *parameter)
{
int32_t err;

if ((DriverHandle) && (PCanDeviceOpen))
  {
  CAN_DRV_REF();
  err = (PCanDeviceOpen)(index, parameter);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanDeviceClose(uint32_t index)
{
int32_t err;

if ((DriverHandle) && (PCanDeviceClose))
  {
  CAN_DRV_REF();
  err = (PCanDeviceClose)(index);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanSetMode(uint32_t index, unsigned char can_op_mode, uint16_t can_command)
{
int32_t err;

if ((DriverHandle) && (PCanSetMode))
  {
  CAN_DRV_REF();
  err = (PCanSetMode)(index, can_op_mode, can_command);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanTransmit(uint32_t index, struct TCanMsg *msg, int32_t count)
{
int32_t err;

if ((DriverHandle) && (PCanTransmit))
  {
  CAN_DRV_REF();
  err = (PCanTransmit)(index, msg, count);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


void CanTransmitClear(uint32_t index)
{
if ((DriverHandle) && (PCanTransmitClear))
  {
  CAN_DRV_REF();
  (PCanTransmitClear)(index);
  CAN_DRV_UNREF();
  }
}


uint32_t CanTransmitGetCount(uint32_t index)
{
int32_t res;

res = 0;
if ((DriverHandle) && (PCanTransmitGetCount))
  {
  CAN_DRV_REF();
  res = (PCanTransmitGetCount)(index);
  CAN_DRV_UNREF();
  }
return(res);
}


int32_t CanTransmitSet(uint32_t index, uint16_t cmd, uint32_t time)
{
int32_t err;

if ((DriverHandle) && (PCanTransmitSet))
  {
  CAN_DRV_REF();
  err = (PCanTransmitSet)(index, cmd, time);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanReceive(uint32_t index, struct TCanMsg *msg, int32_t count)
{
int32_t err;

if ((DriverHandle) && (PCanReceive))
  {
  CAN_DRV_REF();
  err = (PCanReceive)(index, msg, count);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


void CanReceiveClear(uint32_t index)
{
if ((DriverHandle) && (PCanReceiveClear))
  {
  CAN_DRV_REF();
  (PCanReceiveClear)(index);
  CAN_DRV_UNREF();
  }
}


uint32_t CanReceiveGetCount(uint32_t index)
{
uint32_t res;

res = 0;
if ((DriverHandle) && (PCanReceiveGetCount))
  {
  CAN_DRV_REF();
  res = (PCanReceiveGetCount)(index);
  CAN_DRV_UNREF();
  }
return(res);
}


int32_t CanSetSpeed(uint32_t index, uint16_t speed)
{
int32_t err;

if ((DriverHandle) && (PCanSetSpeed))
  {
  CAN_DRV_REF();
  err = (PCanSetSpeed)(index, speed);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanSetSpeedUser(uint32_t index, uint32_t value)
{
int32_t err;

if ((DriverHandle) && (PCanSetSpeedUser))
  {
  CAN_DRV_REF();
  err = (PCanSetSpeedUser)(index, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


char *CanDrvInfo(void)
{
char *res;

res = NULL;
if ((DriverHandle) && (PCanDrvInfo))
  {
  CAN_DRV_REF();
  res = (PCanDrvInfo)();
  CAN_DRV_UNREF();
  }
return(res);
}


char *CanDrvHwInfo(uint32_t index)
{
char *res;

res = NULL;
if ((DriverHandle) && (PCanDrvHwInfo))
  {
  CAN_DRV_REF();
  res = (PCanDrvHwInfo)(index);
  CAN_DRV_UNREF();
  }
return(res);
}


int32_t CanSetFilter(uint32_t index, struct TMsgFilter *msg_filter)
{
int32_t err;

if ((DriverHandle) && (PCanSetFilter))
  {
  CAN_DRV_REF();
  err = (PCanSetFilter)(index, msg_filter);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanGetDeviceStatus(uint32_t index, struct TDeviceStatus *status)
{
int32_t err;

err = 0;
if ((DriverHandle) && (PCanGetDeviceStatus))
  {
  CAN_DRV_REF();
  err = (PCanGetDeviceStatus)(index, status);
  CAN_DRV_UNREF();
  }
else
  {
  if (status)
    {
    status->CanStatus = CAN_STATUS_UNBEKANNT;
    status->FifoStatus = FIFO_STATUS_UNBEKANNT;
    status->DrvStatus = DRV_NOT_LOAD;
    }
  }
return(err);
}


void CanSetPnPEventCallback(void (DRV_CALLBACK_TYPE *event)(uint32_t index, int32_t status))
{
if ((DriverHandle) && (PCanSetPnPEventCallback))
  {
  CAN_DRV_REF();
  (PCanSetPnPEventCallback)(event);
  CAN_DRV_UNREF();
  }
}


void CanSetStatusEventCallback(void (DRV_CALLBACK_TYPE *event)
  (uint32_t index, struct TDeviceStatus *device_status))
{
if ((DriverHandle) && (PCanSetStatusEventCallback))
  {
  CAN_DRV_REF();
  (PCanSetStatusEventCallback)(event);
  CAN_DRV_UNREF();
  }
}


void CanSetRxEventCallback(void (DRV_CALLBACK_TYPE *event)(uint32_t index,
  struct TCanMsg *msg, int32_t count))
{
if ((DriverHandle) && (PCanSetRxEventCallback))
  {
  CAN_DRV_REF();
  (PCanSetRxEventCallback)(event);
  CAN_DRV_UNREF();
  }
}


void CanSetEvents(uint16_t  events)
{
if ((DriverHandle) && (PCanSetEvents))
  {
  CAN_DRV_REF();
  (PCanSetEvents)(events);
  CAN_DRV_UNREF();
  }
}


uint32_t CanEventStatus(void)
{
int32_t err;

if ((DriverHandle) && (PCanEventStatus))
  {
  CAN_DRV_REF();
  err = (PCanEventStatus)();
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


// EX-Funktionen
int32_t CanExGetDeviceCount(int32_t flags)
{
int32_t err;

if ((DriverHandle) && (PCanExGetDeviceCount))
  {
  CAN_DRV_REF();
  err = (PCanExGetDeviceCount)(flags);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetDeviceList(struct TCanDevicesList **devices_list, int32_t flags)
{
int32_t err;

if ((DriverHandle) && (PCanExGetDeviceList))
  {
  CAN_DRV_REF();
  err = (PCanExGetDeviceList)(devices_list, flags);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetDeviceListPerform(int32_t flags)
{
int32_t err;

if ((DriverHandle) && (PCanExGetDeviceListPerform))
  {
  CAN_DRV_REF();
  err = (PCanExGetDeviceListPerform)(flags);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetDeviceListGet(struct TCanDevicesList *item)
{
int32_t err;

if ((DriverHandle) && (PCanExGetDeviceListGet))
  {
  CAN_DRV_REF();
  err = (PCanExGetDeviceListGet)(item);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetDeviceInfo(uint32_t index, struct TCanDeviceInfo *device_info,
  struct TCanInfoVar **hw_info, uint32_t *hw_info_size)
{
int32_t err;

if ((DriverHandle) && (PCanExGetDeviceInfo))
  {
  CAN_DRV_REF();
  err = (PCanExGetDeviceInfo)(index, device_info, hw_info, hw_info_size);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetDeviceInfoPerform(uint32_t index, struct TCanDeviceInfo *device_info)
{
int32_t err;

if ((DriverHandle) && (PCanExGetDeviceInfoPerform))
  {
  CAN_DRV_REF();
  err = (PCanExGetDeviceInfoPerform)(index, device_info);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetDeviceInfoGet(struct TCanInfoVar *item)
{
int32_t err;

if ((DriverHandle) && (PCanExGetDeviceInfoGet))
  {
  CAN_DRV_REF();
  err = (PCanExGetDeviceInfoGet)(item);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


void CanExDataFree(void **data)
{
if ((DriverHandle) && (PCanExDataFree))
  {
  CAN_DRV_REF();
  (PCanExDataFree)(data);
  CAN_DRV_UNREF();
  }
}


int32_t CanExCreateDevice(uint32_t *index, char *options)
{
int32_t err;

if ((DriverHandle) && (PCanExCreateDevice))
  {
  CAN_DRV_REF();
  err = (PCanExCreateDevice)(index, options);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExDestroyDevice(uint32_t *index)
{
int32_t err;

if ((DriverHandle) && (PCanExDestroyDevice))
  {
  CAN_DRV_REF();
  err = (PCanExDestroyDevice)(index);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExCreateFifo(uint32_t index, uint32_t size, TMhsEvent *event_obj, uint32_t event, uint32_t channels)
{
int32_t err;

if ((DriverHandle) && (PCanExCreateFifo))
  {
  CAN_DRV_REF();
  err = (PCanExCreateFifo)(index, size, event_obj, event, channels);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExBindFifo(uint32_t fifo_index, uint32_t device_index, uint32_t bind)
{
int32_t err;

if ((DriverHandle) && (PCanExBindFifo))
  {
  CAN_DRV_REF();
  err = (PCanExBindFifo)(fifo_index, device_index, bind);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


TMhsEvent *CanExCreateEvent(void)
{
TMhsEvent *event;

event = NULL;
if ((DriverHandle) && (PCanExCreateEvent))
  {
  CAN_DRV_REF();
  event = (PCanExCreateEvent)();
  CAN_DRV_UNREF();
  }
return(event);
}


int32_t CanExSetObjEvent(uint32_t index, uint32_t source, TMhsEvent *event_obj, uint32_t event)
{
int32_t err;

if ((DriverHandle) && (PCanExSetObjEvent))
  {
  CAN_DRV_REF();
  err = (PCanExSetObjEvent)(index, source, event_obj, event);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


void CanExSetEvent(TMhsEvent *event_obj, uint32_t event)
{
if ((DriverHandle) && (PCanExSetEvent))
  {
  CAN_DRV_REF();
  (PCanExSetEvent)(event_obj, event);
  CAN_DRV_UNREF();
  }
}


void CanExSetEventAll(uint32_t event)
{
if ((DriverHandle) && (PCanExSetEventAll))
  {
  CAN_DRV_REF();
  (PCanExSetEventAll)(event);
  CAN_DRV_UNREF();
  }
}


void CanExResetEvent(TMhsEvent *event_obj, uint32_t event)
{
if ((DriverHandle) && (PCanExResetEvent))
  {
  CAN_DRV_REF();
  (PCanExResetEvent)(event_obj, event);
  CAN_DRV_UNREF();
  }
}


uint32_t CanExWaitForEvent(TMhsEvent *event_obj, uint32_t timeout)
{
uint32_t res;

res = MHS_TERMINATE;
if ((DriverHandle) && (PCanExWaitForEvent))
  {
  CAN_DRV_REF();
  res = (PCanExWaitForEvent)(event_obj, timeout);
  CAN_DRV_UNREF();
  }
return(res);
}


int32_t CanExInitDriver(char *options)
{
if ((DriverHandle) && (PCanExInitDriver))
  return((PCanExInitDriver)(options));
else
  return(ERR_NO_CAN_DRIVER_LOAD);
}


int32_t CanExSetOptions(uint32_t index, char *options)
{
int32_t err;

if ((DriverHandle) && (PCanExSetOptions))
  {
  CAN_DRV_REF();
  err = (PCanExSetOptions)(index, options);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExSetAsByte(uint32_t index, const char *name, char value)
{
int32_t err;

if ((DriverHandle) && (PCanExSetAsByte))
  {
  CAN_DRV_REF();
  err = (PCanExSetAsByte)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExSetAsWord(uint32_t index, const char *name, int16_t value)
{
int32_t err;

if ((DriverHandle) && (PCanExSetAsWord))
  {
  CAN_DRV_REF();
  err = (PCanExSetAsWord)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExSetAsLong(uint32_t index, const char *name, int32_t value)
{
int32_t err;

if ((DriverHandle) && (PCanExSetAsLong))
  {
  CAN_DRV_REF();
  err = (PCanExSetAsLong)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExSetAsUByte(uint32_t index, const char *name, unsigned char value)
{
int32_t err;

if ((DriverHandle) && (PCanExSetAsUByte))
  {
  CAN_DRV_REF();
  err = (PCanExSetAsUByte)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExSetAsUWord(uint32_t index, const char *name, uint16_t value)
{
int32_t err;

if ((DriverHandle) && (PCanExSetAsUWord))
  {
  CAN_DRV_REF();
  err = (PCanExSetAsUWord)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExSetAsULong(uint32_t index, const char *name, uint32_t value)
{
int32_t err;

if ((DriverHandle) && (PCanExSetAsULong))
  {
  CAN_DRV_REF();
  err = (PCanExSetAsULong)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExSetAsString(uint32_t index, const char *name, char *value)
{
int32_t err;

if ((DriverHandle) && (PCanExSetAsString))
  {
  CAN_DRV_REF();
  err = (PCanExSetAsString)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetAsByte(uint32_t index, const char *name, char *value)
{
int32_t err;

if ((DriverHandle) && (PCanExGetAsByte))
  {
  CAN_DRV_REF();
  err = (PCanExGetAsByte)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetAsWord(uint32_t index, const char *name, int16_t *value)
{
int32_t err;

if ((DriverHandle) && (PCanExGetAsWord))
  {
  CAN_DRV_REF();
  err = (PCanExGetAsWord)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetAsLong(uint32_t index, const char *name, int32_t *value)
{
int32_t err;

if ((DriverHandle) && (PCanExGetAsLong))
  {
  CAN_DRV_REF();
  err = (PCanExGetAsLong)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetAsUByte(uint32_t index, const char *name, unsigned char *value)
{
int32_t err;

if ((DriverHandle) && (PCanExGetAsUByte))
  {
  CAN_DRV_REF();
  err = (PCanExGetAsUByte)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetAsUWord(uint32_t index, const char *name, uint16_t *value)
{
int32_t err;

if ((DriverHandle) && (PCanExGetAsUWord))
  {
  CAN_DRV_REF();
  err = (PCanExGetAsUWord)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetAsULong(uint32_t index, const char *name, uint32_t *value)
{
int32_t err;

if ((DriverHandle) && (PCanExGetAsULong))
  {
  CAN_DRV_REF();
  err = (PCanExGetAsULong)(index, name, value);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetAsString(uint32_t index, const char *name, char **str)
{
int32_t err;

if ((DriverHandle) && (PCanExGetAsString))
  {
  CAN_DRV_REF();
  err = (PCanExGetAsString)(index, name, str);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetAsStringCopy(uint32_t index, const char *name, char *dest, uint32_t *dest_size)
{
int32_t err;

if ((DriverHandle) && (PCanExGetAsStringCopy))
  {
  CAN_DRV_REF();
  err = (PCanExGetAsStringCopy)(index, name, dest, dest_size);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


// <*> neu
int32_t CanExSetAsData(uint32_t index, const char *name, void *data, uint32_t size)
{
int32_t err;

if ((DriverHandle) && (PCanExSetAsData))
  {
  CAN_DRV_REF();
  err = (PCanExSetAsData)(index, name, data, size);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExSetDataPtr(uint32_t index, const char *name, void *data_ptr, uint32_t size)
{
int32_t err;

if ((DriverHandle) && (PCanExSetDataPtr))
  {
  CAN_DRV_REF();
  err = (PCanExSetDataPtr)(index, name, data_ptr, size);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExSetDataBlockCB(uint32_t index, TCanGetDataBlockCB func, void *user_data)
{
int32_t err;

if ((DriverHandle) && (PCanExSetDataBlockCB))
  {
  CAN_DRV_REF();
  err = (PCanExSetDataBlockCB)(index, func, user_data);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetInfoList(uint32_t dev_idx, const char *name, struct TCanInfoVarList **devices_info, int32_t flags)
{
int32_t err;

if ((DriverHandle) && (PCanExGetInfoList))
  {
  CAN_DRV_REF();
  err = (PCanExGetInfoList)(dev_idx, name, devices_info, flags);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetInfoListPerform(uint32_t dev_idx, const char *name, int32_t flags)
{
int32_t err;

if ((DriverHandle) && (PCanExGetInfoListPerform))
  {
  CAN_DRV_REF();
  err = (PCanExGetInfoListPerform)(dev_idx, name, flags);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExGetInfoListGet(uint32_t list_idx, struct TCanInfoVar *item)
{
int32_t err;

if ((DriverHandle) && (PCanExGetInfoListGet))
  {
  CAN_DRV_REF();
  err = (PCanExGetInfoListGet)(list_idx, item);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t MhsCanGetApiHandle(void **api_handle)
{
int32_t err;

if ((DriverHandle) && (PMhsCanGetApiHandle))
  {
  CAN_DRV_REF();
  err = (PMhsCanGetApiHandle)(api_handle);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


// **** CAN-FD
int32_t CanFdTransmit(uint32_t index, struct TCanFdMsg *fd_msg, int32_t count)
{
int32_t err;

if (!DriverHandle)
  err = ERR_NO_CAN_DRIVER_LOAD;
else if (PCanFdTransmit)
  {
  CAN_DRV_REF();
  err = (PCanFdTransmit)(index, fd_msg, count);
  CAN_DRV_UNREF();
  }
else
#ifdef STRICT_CAN_FD_SUPPORT
  err = ERR_NO_CAN_DRIVER_LOAD;
#else
  err = -1; // <*>
#endif
return(err);
}


int32_t CanFdReceive(uint32_t index, struct TCanFdMsg *fd_msg, int32_t count)
{
int32_t err;

if (!DriverHandle)
  err = ERR_NO_CAN_DRIVER_LOAD;
else if (PCanFdReceive)
  {
  CAN_DRV_REF();
  err = (PCanFdReceive)(index, fd_msg, count);
  CAN_DRV_UNREF();
  }
else
#ifdef STRICT_CAN_FD_SUPPORT
  err = ERR_NO_CAN_DRIVER_LOAD;
#else
  err = -1; // <*>
#endif
return(err);
}


#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu
int32_t CanExIoOpen(uint32_t index, const char *name, uint32_t *io_id)
{
int32_t err;

if ((DriverHandle) && (PCanExIoOpen))
  {
  CAN_DRV_REF();
  err = (PCanExIoOpen)(index, name, io_id);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExIoClose(uint32_t io_id)
{
int32_t err;

if ((DriverHandle) && (PCanExIoClose))
  {
  CAN_DRV_REF();
  err = (PCanExIoClose)(io_id);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExIoWrite(uint32_t io_id, void *data, uint32_t *size, uint32_t timeout)
{
int32_t err;

if ((DriverHandle) && (PCanExIoWrite))
  {
  CAN_DRV_REF();
  err = (PCanExIoWrite)(io_id, data, size, timeout);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}


int32_t CanExIoRead(uint32_t io_id, void *data, uint32_t *size, uint32_t timeout)
{
int32_t err;

if ((DriverHandle) && (PCanExIoRead))
  {
  CAN_DRV_REF();
  err = (PCanExIoRead)(io_id, data, size, timeout);
  CAN_DRV_UNREF();
  }
else
  err = ERR_NO_CAN_DRIVER_LOAD;
return(err);
}

#endif

#else

/***************************************************************/
/*  Default Call                                               */
/***************************************************************/
void DefDriverProcedure(int32_t func)
{
(void)func;
}


/***************************************************************/
/*  Default Funktionen                                         */
/***************************************************************/
int32_t DRV_CALLBACK_TYPE DefCanInitDriver(char *options)
{
(void)options;

DefDriverProcedure(1);
return(0);
}


void DRV_CALLBACK_TYPE DefCanDownDriver(void)
{
// DefDriverProcedure(1);
}


int32_t DRV_CALLBACK_TYPE DefCanSetOptions(char *options)
{
(void)options;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanDeviceOpen(uint32_t  index, char *parameter)
{
(void)index;
(void)parameter;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanDeviceClose(uint32_t  index)
{
(void)index;

return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanSetMode(uint32_t index, unsigned char can_op_mode, uint16_t can_command)
{
(void)index;
(void)can_op_mode;
(void)can_command;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanSet(uint32_t index, uint16_t obj_index, uint16_t obj_sub_index, void *data, int32_t size)
{
(void)index;
(void)obj_index;
(void)obj_sub_index;
(void)data;
(void)size;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanGet(uint32_t index, uint16_t obj_index, uint16_t obj_sub_index, void *data, int32_t size)
{
(void)index;
(void)obj_index;
(void)obj_sub_index;
(void)data;
(void)size;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanTransmit(uint32_t index, struct TCanMsg *msg, int32_t count)
{
(void)index;
(void)msg;
(void)count;

DefDriverProcedure(1);
return(0);
}


void DRV_CALLBACK_TYPE DefCanTransmitClear(uint32_t index)
{
(void)index;

DefDriverProcedure(1);
}


uint32_t  DRV_CALLBACK_TYPE DefCanTransmitGetCount(uint32_t index)
{
(void)index;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanTransmitSet(uint32_t index, uint16_t cmd, uint32_t time)
{
(void)index;
(void)cmd;
(void)time;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanReceive(uint32_t index, struct TCanMsg *msg, int32_t count)
{
(void)index;
(void)msg;
(void)count;

DefDriverProcedure(1);
return(0);
}


void DRV_CALLBACK_TYPE DefCanReceiveClear(uint32_t index)
{
(void)index;

DefDriverProcedure(1);
}


uint32_t  DRV_CALLBACK_TYPE DefCanReceiveGetCount(uint32_t index)
{
(void)index;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanSetSpeed(uint32_t index, uint16_t speed)
{
(void)index;
(void)speed;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanSetSpeedUser(uint32_t index, uint32_t value)
{
(void)index;
(void)value;

DefDriverProcedure(1);
return(0);
}


char * DRV_CALLBACK_TYPE DefCanDrvInfo(void)
{
DefDriverProcedure(1);
return(NULL);
}


char * DRV_CALLBACK_TYPE DefCanDrvHwInfo(uint32_t index)
{
(void)index;

DefDriverProcedure(1);
return(NULL);
}


int32_t DRV_CALLBACK_TYPE DefCanSetFilter(uint32_t index, struct TMsgFilter *msg_filter)
{
(void)index;
(void)msg_filter;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanGetDeviceStatus(uint32_t index, struct TDeviceStatus *status)
{
(void)index;
(void)status;

status->CanStatus = CAN_STATUS_UNBEKANNT;
status->FifoStatus = FIFO_STATUS_UNBEKANNT;
status->DrvStatus = DRV_NOT_LOAD;
return(0);
}


void DRV_CALLBACK_TYPE DefCanSetPnPEventCallback(void (DRV_CALLBACK_TYPE *event)(uint32_t index, int32_t status))
{
(void)event;

DefDriverProcedure(1);
}


void DRV_CALLBACK_TYPE DefCanSetStatusEventCallback(void (DRV_CALLBACK_TYPE *event)
  (uint32_t index, struct TDeviceStatus *device_status))
{
(void)event;

DefDriverProcedure(1);
}


void DRV_CALLBACK_TYPE DefCanSetRxEventCallback(void (DRV_CALLBACK_TYPE *event)(uint32_t index,
  struct TCanMsg *msg, int32_t count))
{
(void)event;

DefDriverProcedure(1);
}


void DRV_CALLBACK_TYPE DefCanSetEvents(uint16_t  events)
{
(void)events;

DefDriverProcedure(1);
}


uint32_t DRV_CALLBACK_TYPE DefCanEventStatus(void)
{
DefDriverProcedure(1);
return(1);
}


// EX-Funktionen
int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceCount(int32_t flags)
{
(void)flags;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceList(struct TCanDevicesList **devices_list, int32_t flags)
{
(void)devices_list;
(void)flags;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceListPerform(int32_t flags)
{
(void)flags;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceListGet(struct TCanDevicesList *item)
{
(void)item;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceInfo(uint32_t index, struct TCanDeviceInfo *device_info,
  struct TCanInfoVar **hw_info, uint32_t *hw_info_size)
{
(void)index;
(void)device_info;
(void)hw_info;
(void)hw_info_size;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceInfoPerform(uint32_t index, struct TCanDeviceInfo *device_info)
{
(void)index;
(void)device_info;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetDeviceInfoGet(struct TCanInfoVar *item)
{
(void)item;

DefDriverProcedure(1);
return(0);
}


void DRV_CALLBACK_TYPE DefCanExDataFree(void **data)
{
(void)data;

DefDriverProcedure(1);
}


int32_t DRV_CALLBACK_TYPE DefCanExCreateDevice(uint32_t *index, char *options)
{
(void)index;
(void)options;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExDestroyDevice(uint32_t *index)
{
(void)index;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExCreateFifo(uint32_t index, uint32_t size, TMhsEvent *event_obj, uint32_t event, uint32_t channels)
{
(void)index;
(void)size;
(void)event_obj;
(void)event;
(void)channels;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExBindFifo(uint32_t fifo_index, uint32_t device_index, uint32_t bind)
{
(void)fifo_index;
(void)device_index;
(void)bind;

DefDriverProcedure(1);
return(0);
}


TMhsEvent * DRV_CALLBACK_TYPE DefCanExCreateEvent(void)
{
DefDriverProcedure(1);
return(NULL);
}


int32_t DRV_DRV_CALLBACK_TYPE_TYPE DefCanExSetObjEvent(uint32_t index, uint32_t source, TMhsEvent *event_obj, uint32_t event)
{
(void)index;
(void)source;
(void)event_obj;
(void)event;

DefDriverProcedure(1);
return(0);
}


void DRV_CALLBACK_TYPE DefCanExSetEvent(TMhsEvent *event_obj, uint32_t event)
{
(void)event_obj;
(void)event;

DefDriverProcedure(1);
}


void DRV_CALLBACK_TYPE DefCanExSetEventAll(uint32_t event)
{
(void)event;

DefDriverProcedure(1);
}


void DRV_CALLBACK_TYPE DefCanExResetEvent(TMhsEvent *event_obj, uint32_t event)
{
(void)event_obj;
(void)event;

DefDriverProcedure(1);
}


uint32_t DRV_CALLBACK_TYPE DefCanExWaitForEvent(TMhsEvent *event_obj, uint32_t timeout)
{
(void)event_obj;
(void)timeout;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExInitDriver(char *options)
{
(void)options;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExSetOptions(uint32_t index, char *options)
{
(void)index;
(void)options;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExSetAsByte(uint32_t index, char *name, char value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExSetAsWord(uint32_t index, char *name, int16_t value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExSetAsLong(uint32_t index, char *name, int32_t value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExSetAsUByte(uint32_t index, char *name, unsigned char value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExSetAsUWord(uint32_t index, char *name, uint16_t value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExSetAsULong(uint32_t index, char *name, uint32_t value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExSetAsString(uint32_t index, char *name, char *value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetAsByte(uint32_t index, char *name, char *value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetAsWord(uint32_t index, char *name, int16_t *value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetAsLong(uint32_t index, char *name, int32_t *value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetAsUByte(uint32_t index, char *name, unsigned char *value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetAsUWord(uint32_t index, char *name, uint16_t *value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetAsULong(uint32_t index, char *name, uint32_t *value)
{
(void)index;
(void)name;
(void)value;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetAsString(uint32_t index, char *name, char **str)
{
(void)index;
(void)name;
(void)str;

DefDriverProcedure(1);
return(0);
}


int32_t DRV_CALLBACK_TYPE DefCanExGetAsStringCopy(uint32_t index, char *name, char *dest, uint32_t *dest_size)
{
(void)index;
(void)name;
(void)dest;
(void)dest_size;

DefDriverProcedure(1);
return(0);
}


// **** CAN-FD
int32_t DefCanFdTransmit(uint32_t index, struct TCanFdMsg *fd_msg, int32_t count)
{
(void)index;
(void)fd_msg;
(void)count;

DefDriverProcedure(1);
return(0);
}


int32_t DefCanFdReceive(uint32_t index, struct TCanFdMsg *fd_msg, int32_t count)
{
(void)index;
(void)fd_msg;
(void)count;

DefDriverProcedure(1);
return(0);
}


// **** <*> neu
int32_t DefCanExSetAsData(uint32_t index, char *name, void *data, uint32_t size);
{
(void)index;
(void)name;
(void)data;
(void)size;

DefDriverProcedure(1);
return(0);
}


int32_t DefCanExSetDataPtr(uint32_t index, char *name, void *data_ptr, uint32_t size);
{
(void)index;
(void)name;
(void)data_ptr;
(void)size;

DefDriverProcedure(1);
return(0);
}


int32_t DefCanExSetDataBlockCB(uint32_t index, TCanGetDataBlockCB func, void *user_data)
{
(void)index;
(void)func;
(void)user_data;

DefDriverProcedure(1);
return(0);
}


int32_t DefCanExGetInfoList(uint32_t dev_idx, const char *name, struct TCanInfoVarLists *devices_info, int32_t flags)
{
(void)dev_idx;
(void)name;
(void)devices_info;
(void)flags;

DefDriverProcedure(1);
return(0);
}


int32_t DefCanExGetInfoListPerform(uint32_t dev_idx, const char *name, int32_t flags)
{
(void)dev_idx;
(void)name;
(void)flags;

DefDriverProcedure(1);
return(0);
}


int32_t DefCanExGetInfoListGet(uint32_t list_idx, struct TCanInfoVar *item)
{
(void)list_idx;
(void)item;

DefDriverProcedure(1);
return(0);
}

int32_t DefMhsCanGetApiHandle(void **api_handle)
{
(void)api_handle;

DefDriverProcedure(1);
return(0);
}

#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu
int32_t DefCanExIoOpen(uint32_t index, const char *name, uint32_t *io_id)
{

DefDriverProcedure(1);
return(0);
}


int32_t DefCanExIoClose(uint32_t io_id)
{
(void)io_id;

DefDriverProcedure(1);
return(0);
}


int32_t DefCanExIoWrite(uint32_t io_id, void *data, uint32_t *size, uint32_t timeout)
{
(void)io_id;
(void)data;
(void)size;
(void)timeout;

DefDriverProcedure(1);
return(0);
}


int32_t DefCanExIoRead(uint32_t io_id, void *data, uint32_t *size, uint32_t timeout)
{
(void)io_id;
(void)data;
(void)size;
(void)timeout;

DefDriverProcedure(1);
return(0);
}

#endif

#endif



/***************************************************************/
/*                API Treiber Path ermitteln                   */
/***************************************************************/
#ifndef __WIN32__  // Linux
  #define MAX_PATH_LEN 255

  #define DIR_SEPARATOR '/'
  #define DIR_SEPARATOR_STR "/"

/**************************************************************************/
/*                    C O N S T A N T E N - Linux                         */
/**************************************************************************/
static const char API_DRIVER_DLL[] = {"libmhstcan.so"};

static const char API_PATH_CFG_FILE[] = {"/etc/tiny_can/api.path"};

#define HAVE_API_DRV_DEFAULT_PATH
static const char API_DRV_DEFAULT_PATH[] = {"/opt/tiny_can/can_api"};

#ifdef __LP64__
static const char API_DRIVER_X64_DIR[] = {"x64"};
#else
static const char API_DRIVER_X32_DIR[] = {"x32"};
#endif

#else
  #define MAX_PATH_LEN 255

  #define DIR_SEPARATOR '\\'
  #define DIR_SEPARATOR_STR "\\"

/**************************************************************************/
/*                    C O N S T A N T E N - Windows                       */
/**************************************************************************/
#ifdef _WIN64
static const char REG_TINY_CAN_API[] = {"Software\\Wow6432Node\\Tiny-CAN\\API"};
#else
static const char REG_TINY_CAN_API[] = {"Software\\Tiny-CAN\\API"};
#endif
static const char REG_TINY_CAN_API_PATH_ENTRY[] = {"PATH"};

static const char API_DRIVER_DLL[] = {"mhstcan.dll"};
#ifdef _WIN64
static const char API_DRIVER_X64_DIR[] = {"x64"};
#endif
//typedef char *(*TCanDrvInfoCB)(void);
#endif

/**************************************************************************/
/*                        F U N K T I O N E N                             */
/**************************************************************************/
#if defined(__WIN32__) || defined(LINUX_HAVE_GET_API_DRIVER_PATH)

static char *MhsStpCpy(char *dest, const char *src)
{
register char *d = dest;
register const char *s = src;

do
  *d++ = *s;
while (*s++);
return(d - 1);
}


static char *MhsStrDup(const char *str)
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


static char *MhsStrconcat(const char *string1, ...)
{
size_t l;
va_list args;
char *s, *concat, *ptr;

if (!string1)
  return(NULL);

l = 1 + strlen(string1);
va_start(args, string1);
s = va_arg(args, char*);
while (s)
  {
  l += strlen(s);
  s = va_arg(args, char*);
  }
va_end(args);

concat = (char *)malloc(l);
if (!concat)
  return(NULL);
ptr = concat;

ptr = MhsStpCpy(ptr, string1);
va_start (args, string1);
s = va_arg(args, char*);
while (s)
  {
  ptr = MhsStpCpy(ptr, s);
  s = va_arg(args, char*);
  }
va_end(args);

return(concat);
}


static char *MhsCreateFileName(const char *dir, const char *sub_dir, const char *file_name)
{
if (!dir)
  return(MhsStrDup(file_name));
if (!file_name)
  return(NULL);
if (strchr(file_name, DIR_SEPARATOR))
  return(MhsStrDup(file_name));
else
  {
  if (sub_dir)
    {
    if (dir[strlen(dir)-1] == DIR_SEPARATOR)
      {
      if (sub_dir[strlen(sub_dir)-1] == DIR_SEPARATOR)
        return(MhsStrconcat(dir, sub_dir, file_name, NULL));
      else
        return(MhsStrconcat(dir, sub_dir, DIR_SEPARATOR_STR, file_name, NULL));
      }
    else
      {
      if (sub_dir[strlen(sub_dir)-1] == DIR_SEPARATOR)
        return(MhsStrconcat(dir, DIR_SEPARATOR_STR, sub_dir, file_name, NULL));
      else
        return(MhsStrconcat(dir, DIR_SEPARATOR_STR, sub_dir, DIR_SEPARATOR_STR, file_name, NULL));
      }
    }
  else
    {
    if (dir[strlen(dir)-1] == DIR_SEPARATOR)
      return(MhsStrconcat(dir, file_name, NULL));
    else
      return(MhsStrconcat(dir, DIR_SEPARATOR_STR, file_name, NULL));
    }
  }
}


static int32_t TestApi(const char *file_name)
{
#ifdef __WIN32__
HINSTANCE driver_handle;
#else
void *driver_handle;
#endif
TCanDrvInfo drv_info_cb;

drv_info_cb = NULL;
#ifdef __WIN32__
driver_handle = LoadLibraryA(file_name);
#else
driver_handle = dlopen(file_name, RTLD_LAZY);
#endif
if (driver_handle)
  {
  drv_info_cb = (TCanDrvInfo)MhsGetProcAddress(driver_handle, "CanDrvInfo");
#ifdef __WIN32__
  FreeLibrary(driver_handle);
#else
  dlclose(driver_handle);
#endif
  }
#if defined(MHS_DRV_DEBUG_OUTPUT) && (!defined(__WIN32__))
else
  printf("Error load: %s\n\r, ", dlerror());
#endif
if (drv_info_cb)
  return(0);
else
  return(-1);
}


#endif
#ifdef __WIN32__

static char *MhsRegGetString(HKEY key, const char *value_name)
{
DWORD type, data_size;
char *str;

type = 0;
data_size = 0;
if ((!key) || (!value_name))
  return(NULL);
// Länge des Strings ermitteln
if (RegQueryValueExA(key, value_name, 0, &type, NULL, &data_size) != ERROR_SUCCESS)
  return(NULL);
// Wert ist String ?
if (type != REG_SZ)
  return(NULL);
str = malloc(data_size+1);
if (!str)
  return(NULL);
// String lesen
if (RegQueryValueExA(key, value_name, 0, &type, (LPBYTE)str, &data_size) != ERROR_SUCCESS)
  {
  free(str);
  return(NULL);
  }
str[data_size] = '\0';
return(str);
}


static char *MhsRegReadStringEntry(const char *path, const char *entry)
{
HKEY key;
char *value;

value = NULL;
// HKEY_CURRENT_USER ?
if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, path, 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) // KEY_READ
  {
  value = MhsRegGetString(key, entry);
  RegCloseKey(key);
  }
return(value);
}

/* <*> raus
static int32_t TestApi(const char *file_name)
{
HINSTANCE driver_dll;
TCanDrvInfo drv_info_cb;

drv_info_cb = NULL;
driver_dll = LoadLibraryA(file_name);
if (driver_dll)
  {
  drv_info_cb = (TCanDrvInfo)(void *)GetProcAddress(driver_dll, (LPCSTR)"CanDrvInfo");
  FreeLibrary(driver_dll);
  }
if (drv_info_cb)
  return(0);
else
  return(-1);
} */


static char *GetApiDriverWithPath(const char *driver_file)
{
char *path, *file_name;
char app_file[MAX_PATH_LEN];
char app_path[MAX_PATH_LEN];
char *ptr;

file_name = NULL;
if ((!driver_file ) || (!strlen(driver_file)))
  driver_file = API_DRIVER_DLL;
if (strchr(driver_file, DIR_SEPARATOR))
  return(MhsStrDup(driver_file));
if ((path = MhsRegReadStringEntry(REG_TINY_CAN_API, REG_TINY_CAN_API_PATH_ENTRY)))
  {
#ifdef _WIN64
  file_name = MhsCreateFileName(path, API_DRIVER_X64_DIR, driver_file);
#else
  file_name = MhsCreateFileName(path, NULL, driver_file);
#endif
  if (TestApi(file_name))
    MhsSafeFree(file_name);
  free(path);
  }
if (!file_name)
  {
  // Programm Path ermitteln
  GetModuleFileNameA(GetModuleHandle(NULL), app_file, MAX_PATH_LEN);
  GetFullPathNameA(app_file, sizeof(app_file), app_path, &ptr);
  app_path[ptr-app_path] = '\0';
  file_name = MhsCreateFileName(app_path, NULL, driver_file);
  if (TestApi(file_name))
    MhsSafeFree(file_name);
  }
return(file_name);
}

#else
  #ifdef LINUX_HAVE_GET_API_DRIVER_PATH


static char *GetAppPath(void)
{
char buf[MAX_PATH_LEN];
char *path, *ptr;
int fd;
size_t n;

path = NULL;
SafeSprintf(buf, (MAX_PATH_LEN - 1), "/proc/%d/cmdline", getpid());
fd = open(buf, O_RDONLY);
if (fd > -1)
  {
  if ((n = read(fd, buf, 200)) > 0)
    {
    buf[n] = '\0';
    path = (char *)malloc(PATH_MAX);
    if (!realpath(buf, path))
      MhsSafeFree(path);
    else
      {
      ptr = strrchr(path, DIR_SEPARATOR);
      if (ptr)
        *ptr = '\0';
      else
        MhsSafeFree(path);
      }
    }
  close(fd);
  }
return(path);
}


static char *ReadPathFromCfgFile(void)
{
char buf[MAX_PATH_LEN];
char *path;
int fd;
size_t n;

path = NULL;
fd = open(API_PATH_CFG_FILE, O_RDONLY);
if (fd > -1)
  {
  if ((n = read(fd, buf, 200)) > 0)
    {
    buf[n] = '\0';
    path = MhsStrDup(buf);
    }
  close(fd);
  }
return(path);
}


static char *GetApiDriverWithPath(const char *driver_file)
{
char *path, *file_name;

file_name = NULL;
if ((!driver_file ) || (!strlen(driver_file)))
  driver_file = API_DRIVER_DLL;
if (strchr(driver_file, DIR_SEPARATOR))
  return(MhsStrDup(driver_file));
// **** 1. Pfad vom Konfig File
if ((path = ReadPathFromCfgFile()))
  {
#ifdef MHS_DRV_DEBUG_OUTPUT
  printf("driver path from config file: %s\n\r", path);
#endif
#ifdef __LP64__
  file_name = MhsCreateFileName(path, API_DRIVER_X64_DIR, driver_file);
#else
  file_name = MhsCreateFileName(path, API_DRIVER_X32_DIR, driver_file);
#endif
#ifdef MHS_DRV_DEBUG_OUTPUT
  printf("1. API driver with full path: %s\n\r", file_name);
#endif
  if (TestApi(file_name))
    {
    MhsSafeFree(file_name);
    file_name = MhsCreateFileName(path, NULL, driver_file);
    if (TestApi(file_name))
      MhsSafeFree(file_name);
    }
  free(path);
  }
#ifdef HAVE_API_DRV_DEFAULT_PATH
// **** 2. Default API Pfad
if (!file_name)
  {
#ifdef __LP64__
  file_name = MhsCreateFileName(API_DRV_DEFAULT_PATH, API_DRIVER_X64_DIR, driver_file);
#else
  file_name = MhsCreateFileName(API_DRV_DEFAULT_PATH, API_DRIVER_X32_DIR, driver_file);
#endif
#ifdef MHS_DRV_DEBUG_OUTPUT
  printf("2. API driver with full path: %s\n\r", file_name);
#endif
  if (TestApi(file_name))
    MhsSafeFree(file_name);
  }
if (!file_name)
  {
  file_name = MhsCreateFileName(API_DRV_DEFAULT_PATH, NULL, driver_file);
  if (TestApi(file_name))
    MhsSafeFree(file_name);
  }
#endif
// **** 3. Pfad gleich APP Pfad
if (!file_name)
  {
  if ((path = GetAppPath()))
    {
#ifdef MHS_DRV_DEBUG_OUTPUT
    printf("application path: %s\n\r", path);
#endif
    file_name = MhsCreateFileName(path, NULL, driver_file);
#ifdef MHS_DRV_DEBUG_OUTPUT
    printf("3. API driver with full path: %s\n\r", file_name);
#endif
    if (TestApi(file_name))
      MhsSafeFree(file_name);
    free(path);
    }
  }
if (!file_name)
  file_name = MhsStrDup(driver_file);
return(file_name);
}

  #endif
#endif



