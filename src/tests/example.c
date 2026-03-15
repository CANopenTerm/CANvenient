/** @file example.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on Windows,
 *  similar to libsocketcan on Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <CANvenient.h>

#ifdef _WIN32
#elif __linux__
#include <socketecan.h>
#endif

#include <stdio.h>
#include <stdlib.h>

int main()
{
    struct can_iface* iface[] = {0};
    int count = 0;

    if (0 == can_find_interfaces(iface, &count))
    {
        for (int i = 0; i < count; i++)
        {
            if (iface[i] != NULL)
            {
                printf("[%d] %s ->", i, iface[i]->name);
                iface[i]->baudrate = CAN_BAUD_250K;

                if (0 == can_open(*iface[i]))
                {
                    printf(" opened\n");
                }
                else
                {
                    printf(" failed to open\n");
                }
            }
        }
    }

    can_free_interfaces(iface, count);
    return EXIT_SUCCESS;
}
