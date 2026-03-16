/** @file example.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on Windows.
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
                printf("[%d] %s -> opened\n", i, name);
            }
        }
    }

    can_free_interfaces();
    return EXIT_SUCCESS;
}
