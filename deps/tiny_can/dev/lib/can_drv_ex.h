#ifndef _CAN_DRV_EX_INCLUDES_
  #error cannot include can_drv_ex.h directly, use can_drv.h
#endif

#ifndef __CAN_DRV_EX_H__
#define __CAN_DRV_EX_H__

#ifdef __cplusplus
  extern "C" {
#endif

#define ENABLE_EX_IO_SUPPORT // <*> Experimentell

#define ERR_NO_CAN_DRIVER_LOAD -1000000


#define DEV_LIST_SHOW_TCAN_ONLY 0x01
#define DEV_LIST_SHOW_UNCONNECT 0x02

/***************************************************************/
/* Define Makros                                               */
/***************************************************************/
#define CAN_FEATURE_LOM          0x0001  // Silent Mode (LOM = Listen only Mode)
#define CAN_FEATURE_ARD          0x0002  // Automatic Retransmission disable
#define CAN_FEATURE_TX_ACK       0x0004  // TX ACK (Gesendete Nachrichten bestätigen)
#define CAN_FEATURE_ERROR_MSGS   0x0008  // Error Messages Support
#define CAN_FEATURE_FD_HARDWARE  0x0010  // CAN-FD Hardware
#define CAN_FEATURE_FIFO_OV_MODE 0x0020  // FIFO OV Mode (Auto Clear, OV CAN Messages)
#define CAN_FEATURE_ECU_FLASH    0x0040  // Hardware beschleunigung für ISO-TP ECU-Flash programmierung
#define CAN_FEATURE_CAN_TEST     0x4000  // Tiny-CAN Tester Firmware
#define CAN_FEATURE_HW_TIMESTAMP 0x8000  // Hardware Time Stamp

// (V)alue (T)ype
#define VT_BYTE         0x01
#define VT_UBYTE        0x02
#define VT_WORD         0x03
#define VT_UWORD        0x04
#define VT_LONG         0x05
#define VT_ULONG        0x06

#define VT_BYTE_ARRAY   0x07
#define VT_UBYTE_ARRAY  0x08
#define VT_WORD_ARRAY   0x09
#define VT_UWORD_ARRAY  0x0A
#define VT_LONG_ARRAY   0x0B
#define VT_ULONG_ARRAY  0x0C

#define VT_BYTE_RANGE_ARRAY   0x0D
#define VT_UBYTE_RANGE_ARRAY  0x0E
#define VT_WORD_RANGE_ARRAY   0x0F
#define VT_UWORD_RANGE_ARRAY  0x10
#define VT_LONG_RANGE_ARRAY   0x11
#define VT_ULONG_RANGE_ARRAY  0x12

#define VT_HBYTE  0x40
#define VT_HWORD  0x41
#define VT_HLONG  0x42

#define VT_STREAM    0x80
#define VT_STRING    0x81
#define VT_POINTER   0x82
#define VT_REVISION  0x83
#define VT_DATE      0x84

// MHS (EV)ent (S)ource
#define MHS_EVS_STATUS  1
#define MHS_EVS_PNP     2
#define MHS_EVS_OBJECT  3

#define MHS_EVS_DIN     4
#define MHS_EVS_ENC     5
#define MHS_EVS_KEY     6


#define MHS_TERMINATE 0x80000000


// <*> neu
#define CAN_DATA_ST_IDLE             0
#define CAN_DATA_ST_RUN              1
#define CAN_DATA_ST_FINISH           2
#define CAN_DATA_ST_ERR_ACK_TIMEOUT  -1
#define CAN_DATA_ST_ERR_NACK         -2
#define CAN_DATA_ST_ERR_OVERFLOW     -3     
#define CAN_DATA_ST_ERR_ISOTP        -4

// ISO-TP Flags
#define CAN_DATA_ISOTP_29BIT_ID      0x01
#define CAN_DATA_ISOTP_EXTEND_ADDR   0x02
#define CAN_DATA_ISOTP_TX_PADDING    0x04
#define CAN_DATA_ISOTP_LISTEN_MODE   0x08


// <*> neu
#define TCAN_INFO_KEY_HW_SNR         0x00000000  // Hardware Snr
#define TCAN_INFO_KEY_HW_ID_STR      0x00000001  // Hardware ID String 
#define TCAN_INFO_KEY_HW_BIOS_STR    0x00000002  // Bios ID String
#define TCAN_INFO_KEY_HW_REVISION    0x00000003  // Hardware Revision    
#define TCAN_INFO_KEY_HW_DATE        0x00000004  // Fertigungsdatum
#define TCAN_INFO_KEY_HW_VARIANT_STR 0x00000005  // Hardware Variante  

#define TCAN_INFO_KEY_HW_CAN_COUNT   0x00008000 // Anzahl CAN Interfaces   
#define TCAN_INFO_KEY_HW_CAN_DRV     0x00008010 // Treiber              
#define TCAN_INFO_KEY_HW_CAN_OPTO    0x00008020 // Opto                 
#define TCAN_INFO_KEY_HW_CAN_TERM    0x00008030 // Term                 
#define TCAN_INFO_KEY_HW_CAN_HS      0x00008040 // HighSpeed                
#define TCAN_INFO_KEY_HW_I2C_CNT     0x00008100 // Anzahl I2C Interfaces
#define TCAN_INFO_KEY_HW_SPI_CNT     0x00008200 // Anzahl SPI Interfaces   

#define TCAN_INFO_KEY_FW_ID          0x00001000  // ID                   
#define TCAN_INFO_KEY_FW_ID_STR      0x00001001  // ID String            
#define TCAN_INFO_KEY_FW_VERSION     0x00001002  // Version              
#define TCAN_INFO_KEY_FW_VERSION_STR 0x00001003  // Version String       
#define TCAN_INFO_KEY_FW_AUTOR       0x00001004  // Autor                
#define TCAN_INFO_KEY_FW_OPTIOS      0x00001005  // Optionen             
#define TCAN_INFO_KEY_FW_SNR         0x00001006  // Snr                            

#define TCAN_INFO_KEY_FW_CAN_FLAGS   0x00008001 // CAN Features Flags 
#define TCAN_INFO_KEY_FW_CAN_FLAGS2  0x00008002 // CAN Features Flags2 // <*>
#define TCAN_INFO_KEY_FW_CAN_CLOCK1  0x00008003  // CAN Clock 1
#define TCAN_INFO_KEY_FW_CAN_CLOCK2  0x00008004  // CAN Clock 2
#define TCAN_INFO_KEY_FW_CAN_CLOCK3  0x00008005  // CAN Clock 3
#define TCAN_INFO_KEY_FW_CAN_CLOCK4  0x00008006  // CAN Clock 4
#define TCAN_INFO_KEY_FW_CAN_CLOCK5  0x00008007  // CAN Clock 5
#define TCAN_INFO_KEY_FW_CAN_CLOCK6  0x00008008  // CAN Clock 6
#define TCAN_INFO_KEY_FW_CAN_CLOCK7  0x00008009  // CAN Clock 7
#define TCAN_INFO_KEY_FW_CAN_CLOCK8  0x0000800A  // CAN Clock 8
#define TCAN_INFO_KEY_FW_CAN_CLOCK9  0x0000800B  // CAN Clock 9
#define TCAN_INFO_KEY_FW_CAN_CLOCK10 0x0000800C  // CAN Clock 10
#define TCAN_INFO_KEY_FW_PUFFER_CNT  0x00008050 // Anzahl Interval Puffer
#define TCAN_INFO_KEY_FW_FILTER_CNT  0x00008060 // Anzahl Filter    

#define TCAN_INFO_KEY_OPEN_INDEX     0x01000001
#define TCAN_INFO_KEY_HARDWARE_ID    0x01000002
#define TCAN_INFO_KEY_HARDWARE       0x01000003
#define TCAN_INFO_KEY_VENDOR         0x01000004

#define TCAN_INFO_KEY_DEVICE_NAME    0x01000005
#define TCAN_INFO_KEY_SERIAL_NUMBER  0x01000006

#define TCAN_INFO_KEY_CAN_FEATURES   0x01000007
#define TCAN_INFO_KEY_CAN_CHANNELS   0x01000008
#define TCAN_INFO_KEY_RX_FILTER_CNT  0x01000009 
#define TCAN_INFO_KEY_TX_BUFFER_CNT  0x0100000A
#define TCAN_INFO_KEY_CAN_CLOCKS     0x0100000B
#define TCAN_INFO_KEY_CAN_CLOCK1     0x0100000C
#define TCAN_INFO_KEY_CAN_CLOCK2     0x0100000D
#define TCAN_INFO_KEY_CAN_CLOCK3     0x0100000E
#define TCAN_INFO_KEY_CAN_CLOCK4     0x0100000F
#define TCAN_INFO_KEY_CAN_CLOCK5     0x01000010
#define TCAN_INFO_KEY_CAN_CLOCK6     0x01000011
#define TCAN_INFO_KEY_CAN_CLOCK7     0x01000012
#define TCAN_INFO_KEY_CAN_CLOCK8     0x01000013
#define TCAN_INFO_KEY_CAN_CLOCK9     0x01000014
#define TCAN_INFO_KEY_CAN_CLOCK10    0x01000015

#define TCAN_INFO_KEY_API_VERSION   0x02000001
#define TCAN_INFO_KEY_DLL           0x02000002
#define TCAN_INFO_KEY_CFG_APP       0x02000003

/***************************************************************/
/*  Typen                                                      */
/***************************************************************/
#pragma pack(push, 1)
struct TModulFeatures
  {
  uint32_t CanClock;           // Clock-Frequenz des CAN-Controllers, muss nicht mit
                               // der Clock-Frequenz des Mikrocontrollers übereinstimmen
  uint32_t Flags;              // Unterstützte Features des Moduls:
                               //   Bit    Define-Makro
                               // 0x0001 CAN_FEATURE_LOM          -> Silent Mode (LOM = Listen only Mode)
                               // 0x0002 CAN_FEATURE_ARD          -> Automatic Retransmission disable
                               // 0x0004 CAN_FEATURE_TX_ACK       -> TX ACK (Gesendete Nachrichten bestätigen)
                               // 0x0008 CAN_FEATURE_ERROR_MSGS   -> Error Messages Support
                               // 0x0010 CAN_FEATURE_FD_HARDWARE  -> CAN-FD Hardware
                               // 0x0020 CAN_FEATURE_FIFO_OV_MODE -> FIFO OV Mode (Auto Clear, OV CAN Messages)
                               // 0x0040 CAN_FEATURE_ECU_FLASH    -> Hardware beschleunigung für ISO-TP ECU-Flash programmierung
                               // 0x4000 CAN_FEATURE_CAN_TEST     -> Tiny-CAN Tester Firmware
                               // 0x8000 CAN_FEATURE_HW_TIMESTAMP -> Hardware Time Stamp
  uint32_t CanChannelsCount;   // Anzahl der CAN Schnittstellen, reserviert für
                               // zukünftige Module mit mehr als einer Schnittstelle
  uint32_t HwRxFilterCount;    // Anzahl der zur Verfügung stehenden Receive-Filter
  uint32_t HwTxPufferCount;    // Anzahl der zur Verfügung stehenden Transmit Puffer mit Timer
  };
#pragma pack(pop)

#pragma pack(push, 1)
struct TCanDevicesList
  {
  uint32_t TCanIdx;            // Ist das Device geöffnet ist der Wert auf dem Device-Index
                               // gesetzt, ansonsten ist der Wert auf "INDEX_INVALID" gesetzt.
  uint32_t HwId;               // Ein 32 Bit Schlüssel der die Hardware eindeutig Identifiziert.
                               // Manche Module müssen erst geöffnet werden damit dieser Wert
                               // gesetzt wird
  char DeviceName[255];        // Nur Linux: entspricht den Device Namen des USB-Devices,
                               //            z.B. /dev/ttyUSB0
  char SerialNumber[16];       // Seriennummer des Moduls
  char Description[64];                 // Modul Bezeichnung, z.B. "Tiny-CAN IV-XL",
                                        // muss in den USB-Controller programmiert sein,
                                        // was zur Zeit nur bei den Modulen Tiny-CAN II-XL,
                                        // IV-XL u. M1 der Fall ist.
  struct TModulFeatures ModulFeatures;  // Unterstützte Features des Moduls, nur gültig
                                        // wenn HwId > 0
  };
#pragma pack(pop)

#pragma pack(push, 1)
struct TCanDeviceInfo
  {
  uint32_t HwId;               // Ein 32 Bit Schlüssel der die Hardware eindeutig Identifiziert.
  uint32_t FirmwareVersion;    // Version der Firmware des Tiny-CAN Moduls
  uint32_t FirmwareInfo;       // Informationen zum Stand der Firmware Version
                               //   0 = Unbekannt
                               //   1 = Firmware veraltet, Device kann nicht geöffnet werden
                               //   2 = Firmware veraltet, Funktionsumfang eingeschränkt
                               //   3 = Firmware veraltet, keine Einschränkungen
                               //   4 = Firmware auf Stand
                               //   5 = Firmware neuer als Erwartet
  char SerialNumber[16];       // Seriennummer des Moduls
  char Description[64];        // Modul Bezeichnung, z.B. "Tiny-CAN IV-XL"
  struct TModulFeatures ModulFeatures; // Unterstützte Features des Moduls
  };
#pragma pack(pop)


#pragma pack(push, 1)
struct TCanInfoVar   // <*> geändert von TInfoVar in TCanInfoVar
  {
  uint32_t Key;      // Variablen Schlüssel
  uint32_t Type;     // Variablen Type
  uint32_t Size;     // (Max)Größe der Variable in Byte
  char Data[255];    // Wert der Variable
  };
#pragma pack(pop)

#pragma pack(push, 1)
struct TCanInfoVarList
  {
  uint32_t Size;
  struct TCanInfoVar *List;
  };
#pragma pack(pop)

typedef struct _TMhsEvent TMhsEvent;

#pragma pack(push, 1)
struct _TMhsEvent
  {
  volatile uint32_t Events;
  volatile uint32_t EventsMask;
  volatile int32_t Waiting;
#ifdef __WIN32__
// ****** Windows
  uint32_t WinEventCount;
  HANDLE WinEvent[3];
  CRITICAL_SECTION EventLock;
#else
// ****** Linux
  pthread_cond_t Cond;
  pthread_mutex_t Mutex;
#endif
  };
#pragma pack(pop)


#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu

#define RS485_TX_STATUS_FINISH  1
#define RS485_TX_STATUS_ERROR   2

#define RS485_RX_STATUS_FINISH   1
#define RS485_RX_STATUS_TIMEOUT  2
#define RS485_RX_STATUS_ERROR    3
#define RS485_RX_STATUS_OV       4

typedef struct _TRS485MsData TRS485MsData; 

#pragma pack(push, 1)
struct _TRS485MsData
  {
  uint16_t Flags;
  uint8_t TxStatus;
  uint8_t RxStatus;  
  uint16_t TxSize;
  uint8_t TxData[4096];
  uint16_t RxSize;
  uint8_t RxData[4096];
  };
#pragma pack(pop)

#endif


/***************************************************************/
/*  Callback Funktionstypen                                    */
/***************************************************************/
typedef int32_t (DRV_CALLBACK_TYPE *TCanGetDataBlockCB)(uint8_t *tx_data, uint16_t tx_limit, void *user_data);

#ifdef CAN_API_TRUE_FUNC

int32_t CanExGetDeviceCount(int32_t flags);
int32_t CanExGetDeviceList(struct TCanDevicesList **devices_list, int32_t flags);
int32_t CanExGetDeviceListPerform(int32_t flags);
int32_t CanExGetDeviceListGet(struct TCanDevicesList *item);

int32_t CanExGetDeviceInfo(uint32_t index, struct TCanDeviceInfo *device_info,
  struct TCanInfoVar **hw_info, uint32_t *hw_info_size);
int32_t CanExGetDeviceInfoPerform(uint32_t index, struct TCanDeviceInfo *device_info);
int32_t CanExGetDeviceInfoGet(struct TCanInfoVar *item);
void CanExDataFree(void **data);
int32_t CanExCreateDevice(uint32_t *index, char *options);
int32_t CanExDestroyDevice(uint32_t *index);
int32_t CanExCreateFifo(uint32_t index, uint32_t size, TMhsEvent *event_obj, uint32_t event, uint32_t channels);
int32_t CanExBindFifo(uint32_t fifo_index, uint32_t device_index, uint32_t bind);
TMhsEvent *CanExCreateEvent(void);
int32_t CanExSetObjEvent(uint32_t index, uint32_t source, TMhsEvent *event_obj, uint32_t event);
void CanExSetEvent(TMhsEvent *event_obj, uint32_t event);
void CanExSetEventAll(uint32_t event);
void CanExResetEvent(TMhsEvent *event_obj, uint32_t event);
uint32_t CanExWaitForEvent(TMhsEvent *event_obj, uint32_t timeout);
int32_t CanExInitDriver(char *options);
int32_t CanExSetOptions(uint32_t index, char *options);
int32_t CanExSetAsByte(uint32_t index, const char *name, char value);
int32_t CanExSetAsWord(uint32_t index, const char *name, int16_t value);
int32_t CanExSetAsLong(uint32_t index, const char *name, int32_t value);
int32_t CanExSetAsUByte(uint32_t index, const char *name, unsigned char value);
int32_t CanExSetAsUWord(uint32_t index, const char *name, uint16_t value);
int32_t CanExSetAsULong(uint32_t index, const char *name, uint32_t value);
int32_t CanExSetAsString(uint32_t index, const char *name, char *value);
int32_t CanExGetAsByte(uint32_t index, const char *name, char *value);
int32_t CanExGetAsWord(uint32_t index, const char *name, int16_t *value);
int32_t CanExGetAsLong(uint32_t index, const char *name, int32_t *value);
int32_t CanExGetAsUByte(uint32_t index, const char *name, unsigned char *value);
int32_t CanExGetAsUWord(uint32_t index, const char *name, uint16_t *value);
int32_t CanExGetAsULong(uint32_t index, const char *name, uint32_t *value);
int32_t CanExGetAsString(uint32_t index, const char *name, char **str);
int32_t CanExGetAsStringCopy(uint32_t index, const char *name, char *dest, uint32_t *dest_size);
// **** CAN-FD
int32_t CanFdTransmit(uint32_t index, struct TCanFdMsg *fd_msg, int32_t count);
int32_t CanFdReceive(uint32_t index, struct TCanFdMsg *fd_msg, int32_t count);
// *** <*> neu
int32_t CanExSetAsData(uint32_t index, const char *name, void *data, uint32_t size);
int32_t CanExSetDataPtr(uint32_t index, const char *name, void *data_ptr, uint32_t size);
int32_t CanExSetDataBlockCB(uint32_t index, TCanGetDataBlockCB func, void *user_data);

int32_t CanExGetInfoList(uint32_t dev_idx, const char *name, struct TCanInfoVarList **devices_info, int32_t flags); // <*> neu
int32_t CanExGetInfoListPerform(uint32_t dev_idx, const char *name, int32_t flags);
int32_t CanExGetInfoListGet(uint32_t list_idx, struct TCanInfoVar *item);

int32_t MhsCanGetApiHandle(void **api_handle);

#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu
int32_t CanExIoOpen(uint32_t index, const char *name, uint32_t *io_id);
int32_t CanExIoClose(uint32_t io_id);
int32_t CanExIoWrite(uint32_t io_id, void *data, uint32_t *size, uint32_t timeout);
int32_t CanExIoRead(uint32_t io_id, void *data, uint32_t *size, uint32_t timeout);
#endif

#endif

#if !(defined(CAN_API_TRUE_FUNC)) || defined(CAN_DRV_INCLUDE)

/***************************************************************/
/*  Funktionstypen                                             */
/***************************************************************/
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetDeviceCount)(int32_t flags);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetDeviceList)(struct TCanDevicesList **devices_list, int32_t flags);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetDeviceListPerform)(int32_t flags);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetDeviceListGet)(struct TCanDevicesList *item);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetDeviceInfo)(uint32_t index, struct TCanDeviceInfo *device_info,
  struct TCanInfoVar **hw_info, uint32_t *hw_info_size);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetDeviceInfoPerform)(uint32_t index, struct TCanDeviceInfo *device_info);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetDeviceInfoGet)(struct TCanInfoVar *item);
typedef void (DRV_CALLBACK_TYPE *TCanExDataFree)(void **data);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExCreateDevice)(uint32_t *index, char *options);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExDestroyDevice)(uint32_t *index);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExCreateFifo)(uint32_t index, uint32_t size, TMhsEvent *event_obj,
                 uint32_t event, uint32_t channels);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExBindFifo)(uint32_t fifo_index, uint32_t device_index,
                 uint32_t bind);
typedef TMhsEvent * (DRV_CALLBACK_TYPE *TCanExCreateEvent)(void);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetObjEvent)(uint32_t index, uint32_t source, TMhsEvent *event_obj, uint32_t event);
typedef void (DRV_CALLBACK_TYPE *TCanExSetEvent)(TMhsEvent *event_obj, uint32_t event);
typedef void (DRV_CALLBACK_TYPE *TCanExSetEventAll)(uint32_t event);
typedef void (DRV_CALLBACK_TYPE *TCanExResetEvent)(TMhsEvent *event_obj, uint32_t event);
typedef uint32_t (DRV_CALLBACK_TYPE *TCanExWaitForEvent)(TMhsEvent *event_obj, uint32_t timeout);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExInitDriver)(char *options);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetOptions)(uint32_t index, const char *options);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetAsByte)(uint32_t index, const char *name, char value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetAsWord)(uint32_t index, const char *name, int16_t value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetAsLong)(uint32_t index, const char *name, int32_t value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetAsUByte)(uint32_t index, const char *name, unsigned char value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetAsUWord)(uint32_t index, const char *name, uint16_t value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetAsULong)(uint32_t index, const char *name, uint32_t value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetAsString)(uint32_t index, const char *name, char *value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetAsByte)(uint32_t index, const char *name, char *value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetAsWord)(uint32_t index, const char *name, int16_t *value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetAsLong)(uint32_t index, const char *name, int32_t *value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetAsUByte)(uint32_t index, const char *name, unsigned char *value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetAsUWord)(uint32_t index, const char *name, uint16_t *value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetAsULong)(uint32_t index, const char *name, uint32_t *value);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetAsString)(uint32_t index, const char *name, char **str);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetAsStringCopy)(uint32_t index, const char *name, char *dest, uint32_t *dest_size);
// ****** CAN-FD
typedef int32_t (DRV_CALLBACK_TYPE *TCanFdTransmit)(uint32_t index, struct TCanFdMsg *fd_msg, int32_t count);
typedef int32_t (DRV_CALLBACK_TYPE *TCanFdReceive)(uint32_t index, struct TCanFdMsg *fd_msg, int32_t count);
// ***** <*>
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetAsData)(uint32_t index, const char *name, void *data, uint32_t size);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetDataPtr)(uint32_t index, const char *name, void *data_ptr, uint32_t size);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExSetDataBlockCB)(uint32_t index, TCanGetDataBlockCB func, void *user_data);
// ***** <*> neu 2
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetInfoList)(uint32_t dev_idx, const char *name, struct TCanInfoVarList **devices_info, int32_t flags); // <*> neu
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetInfoListPerform)(uint32_t dev_idx, const char *name, int32_t flags);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExGetInfoListGet)(uint32_t list_idx, struct TCanInfoVar *item);

// ***** <*> neu 3
typedef int32_t (DRV_CALLBACK_TYPE *TMhsCanGetApiHandle)(void **api_handle);

#ifdef ENABLE_EX_IO_SUPPORT  // <*> neu
typedef int32_t (DRV_CALLBACK_TYPE *TCanExIoOpen)(uint32_t index, const char *name, uint32_t *io_id);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExIoClose)(uint32_t io_id);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExIoWrite)(uint32_t io_id, void *data, uint32_t *size, uint32_t timeout);
typedef int32_t (DRV_CALLBACK_TYPE *TCanExIoRead)(uint32_t io_id, void *data, uint32_t *size, uint32_t timeout);
#endif

#endif

#ifndef CAN_API_TRUE_FUNC
/***************************************************************/
/*  Tiny-CAN API Funktionen                                    */
/***************************************************************/
extern TCanExGetDeviceCount CanExGetDeviceCount;
extern TCanExGetDeviceList CanExGetDeviceList;
extern TCanExGetDeviceListPerform CanExGetDeviceListPerform;
extern TCanExGetDeviceListGet CanExGetDeviceListGet;
extern TCanExGetDeviceInfo CanExGetDeviceInfo;
extern TCanExGetDeviceInfoPerform CanExGetDeviceInfoPerform;
extern TCanExGetDeviceInfoGet CanExGetDeviceInfoGet;
extern TCanExDataFree CanExDataFree;
extern TCanExCreateDevice CanExCreateDevice;
extern TCanExDestroyDevice CanExDestroyDevice;
extern TCanExCreateFifo CanExCreateFifo;
extern TCanExBindFifo CanExBindFifo;
extern TCanExCreateEvent CanExCreateEvent;
extern TCanExSetObjEvent CanExSetObjEvent;
extern TCanExSetEvent CanExSetEvent;
extern TCanExSetEventAll CanExSetEventAll;
extern TCanExResetEvent CanExResetEvent;
extern TCanExWaitForEvent CanExWaitForEvent;
extern TCanExInitDriver CanExInitDriver;
extern TCanExSetOptions CanExSetOptions;
extern TCanExSetAsByte CanExSetAsByte;
extern TCanExSetAsWord CanExSetAsWord;
extern TCanExSetAsLong CanExSetAsLong;
extern TCanExSetAsUByte CanExSetAsUByte;
extern TCanExSetAsUWord CanExSetAsUWord;
extern TCanExSetAsULong CanExSetAsULong;
extern TCanExSetAsString CanExSetAsString;
extern TCanExGetAsByte CanExGetAsByte;
extern TCanExGetAsWord CanExGetAsWord;
extern TCanExGetAsLong CanExGetAsLong;
extern TCanExGetAsUByte CanExGetAsUByte;
extern TCanExGetAsUWord CanExGetAsUWord;
extern TCanExGetAsULong CanExGetAsULong;
extern TCanExGetAsString CanExGetAsString;
extern TCanExGetAsStringCopy CanExGetAsStringCopy;

// ****** CAN-FD
extern TCanFdTransmit CanFdTransmit;
extern TCanFdReceive CanFdReceive;
// **** <*> neu
extern TCanExSetAsData CanExSetAsData;
extern TCanExSetDataPtr CanExSetDataPtr;
extern TCanExSetDataBlockCB CanExSetDataBlockCB;

extern TCanExGetInfoList CanExGetInfoList;
extern TCanExGetInfoListPerform CanExGetInfoListPerform;
extern TCanExGetInfoListGet CanExGetInfoListGet;

extern TMhsCanGetApiHandle MhsCanGetApiHandle;

#endif

#ifdef __cplusplus
  }
#endif

#endif
