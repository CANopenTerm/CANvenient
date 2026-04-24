/** @file CANvenient_SocketCAN.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void socketcan_close(int index);

#ifdef __linux__
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <libsocketcan.h>
#include <time.h>
#include <unistd.h>
#endif

#include "CANvenient.h"
#include "CANvenient_internal.h"

int socketcan_find_interfaces(void)
{
#ifdef __linux__

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

        /* Skip if this interface is already registered */
        {
            u32 k;
            int already_registered = 0;
            for (k = 0; k < CAN_MAX_INTERFACES; k++)
            {
                if (can_interface[k].name && strcmp(can_interface[k].name, entry->d_name) == 0)
                {
                    already_registered = 1;
                    break;
                }
            }
            if (already_registered)
            {
                continue;
            }
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

        can_interface[free_index].vendor = CAN_VENDOR_SOCKETCAN;
        can_interface[free_index].opened = 0;
        can_interface[free_index].baudrate = CAN_BAUD_1M;
        {
            int* sock_fd = (int*)malloc(sizeof(int));
            if (NULL == sock_fd)
            {
                closedir(dir);
                return -1;
            }
            *sock_fd = -1;
            can_interface[free_index].internal = sock_fd; /* CAN socket. */
        }
    }

    closedir(dir);

#endif

    return 0;
}

int socketcan_open(int index)
{
#ifdef __linux__

    if (can_interface[index].name)
    {
        struct sockaddr_can addr;
        struct ifreq ifr;
        int buffer_size = 1024 * 1024; /* 1MB */
        int enable_timestamp = 1;
        int* can_socket = can_interface[index].internal;

        if (can_interface[index].opened)
        {
            socketcan_close(index);
        }

        *can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (*can_socket < 0)
        {
            set_error_reason("socketcan_open: failed to create socket");
            return -1;
        }

        setsockopt(*can_socket, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size));
        setsockopt(*can_socket, SOL_SOCKET, SO_TIMESTAMP, &enable_timestamp, sizeof(enable_timestamp));

        strcpy(ifr.ifr_name, can_interface[index].name);
        if (ioctl(*can_socket, SIOCGIFINDEX, &ifr) < 0)
        {
            set_error_reason("socketcan_open: ioctl SIOCGIFINDEX failed");
            return -1;
        }

        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(*can_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        {
            set_error_reason("socketcan_open: bind failed");
            return 1;
        }

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000; /* 1 ms receive timeout */
        setsockopt(*can_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        can_interface[index].opened = 1;

        return 0;
    }
    else
    {
        set_error_reason("socketcan_open: invalid CAN interface");
        return -1;
    }

#else
    (void)index;
    return -1;
#endif
}

void socketcan_close(int index)
{
#ifdef __linux__

    if (can_interface[index].name && can_interface[index].opened)
    {
        int* can_socket = can_interface[index].internal;
        close(*can_socket);
        *can_socket = -1;
        can_interface[index].opened = 0;
    }

#else
    (void)index;
#endif
}

int socketcan_update(int index)
{
#ifdef __linux__

    struct ifreq ifr;
    int tmp_sock;

    if (NULL == can_interface[index].name)
    {
        return -1;
    }

    tmp_sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (tmp_sock < 0)
    {
        return -1;
    }

    strcpy(ifr.ifr_name, can_interface[index].name);
    if (ioctl(tmp_sock, SIOCGIFINDEX, &ifr) < 0)
    {
        close(tmp_sock);
        can_release(index);
        return -1;
    }

    close(tmp_sock);
    return 0;

#else
    set_error_reason("SocketCAN driver is only supported on Linux.");
    (void)index;
    return -1;
#endif
}

int socketcan_set_baudrate(int index, enum can_baudrate baud)
{
#ifdef __linux__

    u32 bitrate = 1000000;

    switch (baud)
    {
        case CAN_BAUD_1M:
            bitrate = 1000000;
            break;
        case CAN_BAUD_800K:
            bitrate = 800000;
            break;
        case CAN_BAUD_500K:
            bitrate = 500000;
            break;
        case CAN_BAUD_250K:
            bitrate = 250000;
            break;
        case CAN_BAUD_125K:
            bitrate = 125000;
            break;
        case CAN_BAUD_100K:
            bitrate = 100000;
            break;
        case CAN_BAUD_95K:
            bitrate = 95000;
            break;
        case CAN_BAUD_83K:
            bitrate = 83000;
            break;
        case CAN_BAUD_50K:
            bitrate = 50000;
            break;
        case CAN_BAUD_47K:
            bitrate = 47000;
            break;
        case CAN_BAUD_33K:
            bitrate = 33000;
            break;
        case CAN_BAUD_20K:
            bitrate = 20000;
            break;
        case CAN_BAUD_10K:
            bitrate = 10000;
            break;
        case CAN_BAUD_5K:
            bitrate = 5000;
            break;
    }

    return can_set_bitrate(can_interface[index].name, bitrate);

#else
    (void)index;
    (void)baud;
    return -1;
#endif
}

int socketcan_send(int index, const struct can_frame* frame)
{
#ifdef __linux__

    int* can_socket = can_interface[index].internal;
    long num_bytes;
    struct timespec ts = {0};

    nanosleep(&ts, NULL);

    if (can_interface[index].vendor != CAN_VENDOR_SOCKETCAN)
    {
        set_error_reason("socketcan_send: invalid CAN interface");
        return -1;
    }

    num_bytes = write(*can_socket, frame, sizeof(struct can_frame));

    ts.tv_sec = 0;
    ts.tv_nsec = 1000000; // 1 ms = 1,000,000 ns
    nanosleep(&ts, NULL);

    if (-1 == num_bytes)
    {
        set_error_reason("socketcan_send: write failed");
        return -1;
    }

    return 0;

#else
    (void)index;
    (void)frame;
    return -1;
#endif
}

int socketcan_recv(int index, struct can_frame* frame, u64* timestamp)
{
#ifdef __linux__

    struct msghdr msg;
    struct iovec iov;
    char ctrlmsg[CMSG_SPACE(sizeof(struct timeval))];
    struct cmsghdr* cmsg;
    const struct timeval* tv;
    int nbytes;

    if (can_interface[index].vendor != CAN_VENDOR_SOCKETCAN)
    {
        set_error_reason("socketcan_recv: invalid CAN interface");
        return -1;
    }

    iov.iov_base = frame;
    iov.iov_len = sizeof(struct can_frame);

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &ctrlmsg;
    msg.msg_controllen = sizeof(ctrlmsg);
    msg.msg_flags = 0;

    nbytes = recvmsg(*(int*)can_interface[index].internal, &msg, 0);
    if (nbytes < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            set_error_reason("socketcan_recv: timeout, no data received");
        }
        else
        {
            set_error_reason("socketcan_recv: recvmsg failed");
        }

        return -1;
    }

    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg))
    {
        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SO_TIMESTAMP)
        {
            tv = (struct timeval*)CMSG_DATA(cmsg);
            *timestamp = tv->tv_sec * 1000000ULL + tv->tv_usec;
            break;
        }
    }

    return 0;

#else
    (void)index;
    (void)frame;
    (void)timestamp;
    return -1;
#endif
}
