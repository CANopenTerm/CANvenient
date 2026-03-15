/** @file CANvenient.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on Windows,
 *  similar to libsocketcan on Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#ifndef CANVENIENT_H
#define CANVENIENT_H

#ifdef _WIN32
#define CANVENIENT_API __declspec(dllexport)

#include <limits.h>

#if UCHAR_MAX == 0xffU
typedef unsigned char u8;
#else
#error "No 8-bit unsigned type available"
#endif

#if USHRT_MAX == 0xffffU
typedef unsigned short u16;
#elif UINT_MAX == 0xffffU
typedef unsigned int u16;
#else
#error "No 16-bit unsigned type available"
#endif

#if UINT_MAX == 0xffffffffU
typedef unsigned int u32;
#elif ULONG_MAX == 0xffffffffUL
typedef unsigned long u32;
#elif USHRT_MAX == 0xffffffffU
typedef unsigned short u32;
#else
#error "No 32-bit unsigned type available"
#endif

#if ULONG_MAX == 0xffffffffffffffffUL
typedef unsigned long u64;
#elif defined(_WIN32) && defined(_MSC_VER)
typedef unsigned __int64 u64;
#else
typedef unsigned long long u64;
#endif

#elif __linux__
#define CANVENIENT_API __attribute__((visibility("default")))
#endif

/* CAN message flags */
#define CAN_MSG_FLAG_FD 0x01  /* CAN FD frame */
#define CAN_MSG_FLAG_BRS 0x02 /* Bit rate switch (CAN FD) */
#define CAN_MSG_FLAG_ESI 0x04 /* Error state indicator (CAN FD) */
#define CAN_MSG_FLAG_RTR 0x08 /* Remote transmission request */
#define CAN_MSG_FLAG_EXT 0x10 /* Extended frame format (29-bit ID) */

#ifndef _UAPI_CAN_NETLINK_H
#define _UAPI_CAN_NETLINK_H

/*
 * CAN message frame.
 */
struct can_frame
{
    u64 timestamp;  /* Timestamp in microseconds */
    u32 id;         /* 11 or 29 bit identifier */
    u8 flags;       /* CAN frame flags (FD, BRS, ESI, RTR, etc.) */
    u8 dlc;         /* Data length code: number of bytes of data (0..8 for CAN, 0..64 for CAN FD) */
    u8 data[64];    /* CAN frame payload (0..8 bytes for CAN, 0..64 bytes for CAN FD) */
    u8 reserved[2]; /* Padding for alignment */
};

/*
 * CAN bit-timing parameters
 *
 * For further information, please read chapter "8 BIT TIMING
 * REQUIREMENTS" of the "Bosch CAN Specification version 2.0"
 * at http://www.semiconductors.bosch.de/pdf/can2spec.pdf.
 */
struct can_bittiming
{
    u32 bitrate;      /* Bit-rate in bits/second */
    u32 sample_point; /* Sample point in one-tenth of a percent */
    u32 tq;           /* Time quanta (TQ) in nanoseconds */
    u32 prop_seg;     /* Propagation segment in TQs */
    u32 phase_seg1;   /* Phase buffer segment 1 in TQs */
    u32 phase_seg2;   /* Phase buffer segment 2 in TQs */
    u32 sjw;          /* Synchronisation jump width in TQs */
    u32 brp;          /* Bit-rate prescaler */
};

/*
 * CAN harware-dependent bit-timing constant
 *
 * Used for calculating and checking bit-timing parameters
 */
struct can_bittiming_const
{
    char name[16]; /* Name of the CAN controller hardware */
    u32 tseg1_min; /* Time segement 1 = prop_seg + phase_seg1 */
    u32 tseg1_max;
    u32 tseg2_min; /* Time segement 2 = phase_seg2 */
    u32 tseg2_max;
    u32 sjw_max; /* Synchronisation jump width */
    u32 brp_min; /* Bit-rate prescaler */
    u32 brp_max;
    u32 brp_inc;
};

/*
 * CAN clock parameters
 */
struct can_clock
{
    u32 freq; /* CAN system clock frequency in Hz */
};

/*
 * CAN operational and error states
 */
enum can_state
{
    CAN_STATE_ERROR_ACTIVE = 0, /* RX/TX error count < 96 */
    CAN_STATE_ERROR_WARNING,    /* RX/TX error count < 128 */
    CAN_STATE_ERROR_PASSIVE,    /* RX/TX error count < 256 */
    CAN_STATE_BUS_OFF,          /* RX/TX error count >= 256 */
    CAN_STATE_STOPPED,          /* Device is stopped */
    CAN_STATE_SLEEPING,         /* Device is sleeping */
    CAN_STATE_MAX
};

/*
 * CAN bus error counters
 */
struct can_berr_counter
{
    u16 txerr;
    u16 rxerr;
};

/*
 * CAN controller mode
 */
struct can_ctrlmode
{
    u32 mask;
    u32 flags;
};

#define CAN_CTRLMODE_LOOPBACK 0x01       /* Loopback mode */
#define CAN_CTRLMODE_LISTENONLY 0x02     /* Listen-only mode */
#define CAN_CTRLMODE_3_SAMPLES 0x04      /* Triple sampling mode */
#define CAN_CTRLMODE_ONE_SHOT 0x08       /* One-Shot mode */
#define CAN_CTRLMODE_BERR_REPORTING 0x10 /* Bus-error reporting */
#define CAN_CTRLMODE_FD 0x20             /* CAN FD mode */
#define CAN_CTRLMODE_PRESUME_ACK 0x40    /* Ignore missing CAN ACKs */

/*
 * CAN device statistics
 */
struct can_device_stats
{
    u32 bus_error;        /* Bus errors */
    u32 error_warning;    /* Changes to error warning state */
    u32 error_passive;    /* Changes to error passive state */
    u32 bus_off;          /* Changes to bus off state */
    u32 arbitration_lost; /* Arbitration lost errors */
    u32 restarts;         /* CAN controller re-starts */
};

/*
 * CAN netlink interface
 */
enum
{
    IFLA_CAN_UNSPEC,
    IFLA_CAN_BITTIMING,
    IFLA_CAN_BITTIMING_CONST,
    IFLA_CAN_CLOCK,
    IFLA_CAN_STATE,
    IFLA_CAN_CTRLMODE,
    IFLA_CAN_RESTART_MS,
    IFLA_CAN_RESTART,
    IFLA_CAN_BERR_COUNTER,
    IFLA_CAN_DATA_BITTIMING,
    IFLA_CAN_DATA_BITTIMING_CONST,
    __IFLA_CAN_MAX
};

#define IFLA_CAN_MAX (__IFLA_CAN_MAX - 1)

#endif /* _UAPI_CAN_NETLINK_H */

#ifdef __linux__
struct rtnl_link_stats64; /* from <linux/if_link.h> */
#else
struct rtnl_link_stats64
{
    u32 rx_packets;
    u32 tx_packets;
    u32 rx_bytes;
    u32 tx_bytes;
    u32 rx_errors;
    u32 tx_errors;
    u32 rx_dropped;
    u32 tx_dropped;
    u32 multicast;
    u32 collisions;
    u32 rx_length_errors;
    u32 rx_over_errors;
    u32 rx_crc_errors;
    u32 rx_frame_errors;
    u32 rx_fifo_errors;
    u32 rx_missed_errors;
    u32 tx_aborted_errors;
    u32 tx_carrier_errors;
    u32 tx_fifo_errors;
    u32 tx_heartbeat_errors;
    u32 tx_window_errors;
    u32 rx_compressed;
    u32 tx_compressed;
};
#endif

#ifndef _socketcan_netlink_h
#define _socketcan_netlink_h

/* libsocketcan compatible API. */
CANVENIENT_API int can_do_restart(const char* name);
CANVENIENT_API int can_do_stop(const char* name);
CANVENIENT_API int can_do_start(const char* name);

CANVENIENT_API int can_set_restart_ms(const char* name, u32 restart_ms);
CANVENIENT_API int can_set_bittiming(const char* name, struct can_bittiming* bt);
CANVENIENT_API int can_set_canfd_bittiming(const char* name, struct can_bittiming* bt, struct can_bittiming* dbt);
CANVENIENT_API int can_set_ctrlmode(const char* name, struct can_ctrlmode* cm);
CANVENIENT_API int can_set_bitrate(const char* name, u32 bitrate);
CANVENIENT_API int can_set_bitrate_samplepoint(const char* name, u32 bitrate, u32 sample_point);
CANVENIENT_API int can_set_canfd_bitrates_samplepoint(const char* name, u32 bitrate, u32 sample_point, u32 dbitrate, u32 dsample_point);

CANVENIENT_API int can_get_restart_ms(const char* name, u32* restart_ms);
CANVENIENT_API int can_get_bittiming(const char* name, struct can_bittiming* bt);
CANVENIENT_API int can_get_data_bittiming(const char* name, struct can_bittiming* dbt);
CANVENIENT_API int can_get_ctrlmode(const char* name, struct can_ctrlmode* cm);
CANVENIENT_API int can_get_state(const char* name, int* state);
CANVENIENT_API int can_get_clock(const char* name, struct can_clock* clock);
CANVENIENT_API int can_get_bittiming_const(const char* name, struct can_bittiming_const* btc);
CANVENIENT_API int can_get_data_bittiming_const(const char* name, struct can_bittiming_const* dbtc);
CANVENIENT_API int can_get_berr_counter(const char* name, struct can_berr_counter* bc);
CANVENIENT_API int can_get_device_stats(const char* name, struct can_device_stats* cds);
CANVENIENT_API int can_get_link_stats(const char* name, struct rtnl_link_stats64* rls);

#endif /* _socketcan_netlink_h */

/* CANvenient-specific API. */
CANVENIENT_API int can_find_id(int* id, const char* name);
CANVENIENT_API int can_find_name(char* name, const int id);

CANVENIENT_API int can_open(const char* name);
CANVENIENT_API int can_open_fd(const char* name);
CANVENIENT_API int can_close(const char* name);

CANVENIENT_API int can_send(const char* name, struct can_frame* frame);
CANVENIENT_API int can_recv(const char* name, struct can_frame* frame);

#endif /* CANVENIENT_H */
