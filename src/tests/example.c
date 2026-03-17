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
                struct can_frame out_frame = {0};
                struct can_frame in_frame = {0};

                printf("[%d] %s -> opened\n", i, name);

                out_frame.can_id = 0x123;
                out_frame.data[0] = 0xDE;
                out_frame.data[1] = 0xAD;
                out_frame.data[2] = 0xBE;
                out_frame.data[3] = 0xEF;
                out_frame.can_dlc = 4;

                if (can_send(i, &out_frame) < 0)
                {
                    printf("Failed to send CAN frame on interface %d\n", i);
                }

                if (can_recv(i, &in_frame) < 0)
                {
                    printf("Failed to receive CAN frame on interface %d\n", i);
                }
                else
                {
                    printf("%03X [%d] %02X %02X %02X %02X\n", in_frame.can_id, in_frame.can_dlc, in_frame.data[0], in_frame.data[1], in_frame.data[2], in_frame.data[3]);
                }
            }
        }
    }

    /* Implicitly calls can_close() for all opened interfaces. */
    can_free_interfaces();
    return EXIT_SUCCESS;
}
