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

int main()
{
    if (0 == can_find_interfaces())
    {
        for (int i = 0; i < CAN_MAX_INTERFACES; i++)
        {
            char name[256] = {0};
            can_get_name(i, name, sizeof(name));

            if (0 == can_open(i))
            {
                struct can_frame frame = {0};

                printf("[%d] %s -> opened\n", i, name);

                frame.can_id = 0x123;
                frame.data[0] = 0xDE;
                frame.data[1] = 0xAD;
                frame.data[2] = 0xBE;
                frame.data[3] = 0xEF;
                frame.can_dlc = 4;

                if (can_send(i, &frame) < 0)
                {
                    printf("Failed to send CAN frame on interface %d\n", i);
                }
            }
        }
    }

    /* Implicitly calls can_close() for all opened interfaces. */
    can_free_interfaces();
    return EXIT_SUCCESS;
}
