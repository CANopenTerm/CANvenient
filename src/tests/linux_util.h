#ifndef __LINUX_UTIL_H__
#define __LINUX_UTIL_H__

#ifdef __cplusplus
  extern "C" {
#endif

#ifndef _WIN32

int UtilInit(void);
int KeyHit(void);
//char getch(void);

#endif

#ifdef __cplusplus
  }
#endif

#endif
