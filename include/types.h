#ifndef LIBDRAWABLE_TYPES_H
#define LIBDRAWABLE_TYPES_H

#include <stdlib.h>
#include <stdint.h>

#define UOffsetPointer(ptr, offset) ((void*)((char *)(ptr) + (offset)))

#define LIBDRAWABLE_API EXTERN API

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct { float x, y; } UVector2;
    typedef struct { float x, y, z; } UVector3;
    typedef struct { float x, y, z, w; } UVector4;

    typedef enum UStreamEndian {
        ENDIAN_LITTLE = 0,
        ENDIAN_BIG
    } UStreamEndian;

    typedef enum UStreamError {
        STREAM_ERROR_SUCCESS = 0,
        STREAM_ERROR_OOB
    } UStreamError;

    /* The LOD levels that drawables can define. */
    typedef enum EDrawableLod {
      LOD_HIGH = 0,
      LOD_MED,
      LOD_LOW,
      LOD_VLOW,
  
      LOD_MAX
    } EDrawableLod;

#ifdef __cplusplus
}
#endif

#endif