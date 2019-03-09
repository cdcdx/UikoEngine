#pragma once

#include <windows.h>

#ifdef	__cplusplus
extern "C" {
#endif

//Base64±àÂë
size_t Base64_Encode(char *pDest, const char *pSrc, size_t srclen);
//Base64½âÂë
size_t Base64_Decode(char *pDest, const char *pSrc, size_t srclen);

#ifdef  __cplusplus
}
#endif
