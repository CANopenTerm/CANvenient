#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifdef __cplusplus
  extern "C" {
#endif

#if defined(WIN32) || defined(_WIN32)
  #ifndef __WIN32__
    #define __WIN32__
  #endif
#endif

#ifdef __WIN32__
  // ****** Windows
  #define ATTRIBUTE_INTERNAL
#elif __APPLE__
  // ****** Apple
  #define ATTRIBUTE_INTERNAL __attribute((visibility("hidden")))
#else
  // ****** Linux
  #define ATTRIBUTE_INTERNAL
  // replace -fvisibility=hidden
  //#define ATTRIBUTE_INTERNAL __attribute((visibility("internal")))
#endif

#ifdef __cplusplus
  }
#endif

#endif
