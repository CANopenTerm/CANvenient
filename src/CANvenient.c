/** @file CANvenient.c
 *
 *  CANvenient is an abstraction layer for multiple CAN APIs on Windows,
 *  similar to libsocketcan on Linux.
 *
 *  Copyright (c) 2026, Michael Fitzmayer. All rights reserved.
 *  SPDX-License-Identifier: MIT
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CANvenient.h"

#ifdef _WIN32
#include <windows.h>

#include <PCANBasic.h>
#elif __linux__
#endif

CANVENIENT_API int can_do_init(int index)
{
    (void)index;
    return 0;
}
