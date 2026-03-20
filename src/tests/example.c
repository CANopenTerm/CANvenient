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
            if (name[0] != '\0')
            {
                if (0 == can_open(i))
                {
                    printf("Opened CAN interface: %s\n", name);
                }
                else
                {
                    char error_reason[256] = {0};
                    can_get_error(error_reason, sizeof(error_reason));
                    printf("Failed to open CAN interface %s: %s\n", name, error_reason);
                }
            }
        }
    }

    /* Implicitly calls can_close() for all opened interfaces. */
    can_free_interfaces();
    return EXIT_SUCCESS;
}
