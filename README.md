# CANvenient

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/cf588c93f8e4453e985bd4eb8464e63a)](https://app.codacy.com/gh/CANopenTerm/CANvenient/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![Continuous Integration](https://github.com/CANopenTerm/CANvenient/actions/workflows/continuous-integration.yml/badge.svg)](https://github.com/CANopenTerm/CANvenient/actions/workflows/continuous-integration.yml)

CANvenient is an abstraction layer for multiple CAN APIs on Windows and Linux.
It provides a unified interface for CAN communication, allowing developers to
write code that is portable across different platforms and CAN hardware.

## Supported Back-Ends

The following back-ends are currently implemented:

- Ixxat VCI
- Kvaser CANlib
- PCAN-Basic
- SocketCAN
- Softing CAN Layer 2

## Hardware Contributions

Reliable behavior of CANvenient as a CAN abstraction layer depends on
validation across a broad range of hardware implementations. Limiting testing
to a small set of interfaces increases the risk of vendor-specific
inconsistencies and unhandled edge cases.

Donations of unused or surplus CAN adapters - such as PEAK PCAN-USB, Ixxat
USB-to-CAN, Kvaser Leaf, or comparable devices - enable:

- Validation of CANvenient against real hardware

- Detection of vendor-specific deviations and timing differences

- Improved backend integration across multiple interfaces

- Broader regression testing and increased platform coverage

CANvenient is an open-source project with no external funding; expanding the
set of supported interfaces strengthens portability and ensures consistent
behavior across heterogeneous CAN environments. Contributors can initiate
hardware donations by opening an issue or contacting the maintainers directly.
