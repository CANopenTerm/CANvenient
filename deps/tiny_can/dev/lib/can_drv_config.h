#ifndef __CAN_DRV_CONFIG_H__
#define __CAN_DRV_CONFIG_H__

#ifndef STRICT_CAN_FD_SUPPORT
  //#define STRICT_CAN_FD_SUPPORT
#endif

#ifndef CAN_API_TRUE_FUNC
  #define CAN_API_TRUE_FUNC
#endif

#ifndef DRV_REF_LOCKING
  #define DRV_REF_LOCKING
#endif

#ifndef __WIN32__
  #ifndef LINUX_HAVE_GET_API_DRIVER_PATH
    #define LINUX_HAVE_GET_API_DRIVER_PATH
  #endif
#endif

#ifndef MHS_DRV_DEBUG_OUTPUT
  // #define MHS_DRV_DEBUG_OUTPUT
#endif

#endif
