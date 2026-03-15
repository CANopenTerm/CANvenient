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
    return EXIT_SUCCESS;
}
