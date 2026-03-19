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

    /* Implicitly calls can_close() for all opened interfaces. */
    can_free_interfaces();
    return EXIT_SUCCESS;
}
