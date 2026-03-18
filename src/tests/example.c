/** @file example.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on
 *  Windows and Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <CANvenient.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#define sleep_ms(ms) Sleep(ms)
#else
#include <unistd.h>
#define sleep_ms(ms) usleep((ms) * 1000)
#endif

int main()
{
    if (0 == can_find_interfaces())
    {
        for (int i = 0; i < CAN_MAX_INTERFACES; i++)
        {
            char name[256] = {0};
            can_get_name(i, name, sizeof(name));
            if (name[0] != '\0')
            {
                if (0 == can_open(i))
                {
                    printf("Opened CAN interface: %s\n", name);
                }
                else
                {
                    char error_reason[256] = {0};
                    can_get_error_reason(error_reason, sizeof(error_reason));
                    printf("Failed to open CAN interface %s: %s\n", name, error_reason);
                }
            }
        }
    }

    struct can_frame out = {0};
    struct can_frame in = {0};
    u64 timestamp = 0;

    out.can_id = 0x123;
    out.can_dlc = 2;
    out.data[0] = 0xAB;
    out.data[1] = 0xCD;

    puts("Sending CAN frames from interface 0 and receiving on interface 1...");
    for (int i = 0; i < 10; i++)
    {
        if (can_send(0, &out) < 0)
        {
            char error_reason[256] = {0};
            can_get_error_reason(error_reason, sizeof(error_reason));
            printf("%s\n", error_reason);
        }

        if (can_recv(1, &in, &timestamp) < 0)
        {
            char error_reason[256] = {0};
            can_get_error_reason(error_reason, sizeof(error_reason));
            // printf("%s\n", error_reason);
        }
        else
        {
            printf("Received CAN frame with ID 0x%X, DLC %u, data: %02X %02X\n",
                   in.can_id, in.can_dlc, in.data[0], in.data[1]);
        }

        sleep_ms(100);
    }

    /* Implicitly calls can_close() for all opened interfaces. */
    can_free_interfaces();
    return EXIT_SUCCESS;
}
