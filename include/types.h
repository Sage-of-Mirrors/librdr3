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

    /* The various vertex attributes that a model can have. */
    typedef enum EVertexAttribute {
        VAT_POSITION = 0,
        VAT_POSITION1,
        VAT_POSITION2,
        VAT_POSITION3,
        VAT_NORMAL,
        VAT_NORMAL1,
        VAT_NORMAL2,
        VAT_NORMAL3,
        VAT_TANGENT,
        VAT_TANGENT1,
        VAT_TANGENT2,
        VAT_TANGENT3,
        VAT_BINORMAL,
        VAT_BINORMAL1,
        VAT_BINORMAL2,
        VAT_BINORMAL3,
        VAT_BLENDWEIGHT,
        VAT_BLENDWEIGHT1,
        VAT_BLENDWEIGHT2,
        VAT_BLENDWEIGHT3,
        VAT_BLENDINDEX,
        VAT_BLENDINDEX1,
        VAT_BLENDINDEX2,
        VAT_BLENDINDEX3,
        VAT_COLOR,
        VAT_COLOR1,
        VAT_COLOR2,
        VAT_COLOR3,
        VAT_TEXCOORD,
        VAT_TEXCOORD1,
        VAT_TEXCOORD2,
        VAT_TEXCOORD3,
        VAT_TEXCOORD4,
        VAT_TEXCOORD5,
        VAT_TEXCOORD6,
        VAT_TEXCOORD7,
        VAT_TEXCOORD8,
        VAT_TEXCOORD9,
        VAT_TEXCOORD10,
        VAT_TEXCOORD11,
        VAT_TEXCOORD12,
        VAT_TEXCOORD13,
        VAT_TEXCOORD14,
        VAT_TEXCOORD15,
        VAT_TEXCOORD16,
        VAT_TEXCOORD17,
        VAT_TEXCOORD18,
        VAT_TEXCOORD19,
        VAT_TEXCOORD20,
        VAT_TEXCOORD21,
        VAT_TEXCOORD22,
        VAT_TEXCOORD23,

        VAT_MAX
    } EVertexAttribute;

    /* The formats that vertex data can be stored in. */
    typedef enum EVertexFormat {
        VFT_NONE = 0,
        VFT_RGBA32_FLOAT = 2,
        VFT_RGB32_FLOAT = 6,
        VFT_RGBA16_FLOAT = 10,
        VFT_RG32 = 16,
        VFT_RGB10A2 = 24,
        VFT_RGBA8_UNORM = 28,
        VFT_RGBA8_UINT = 30,
        VFT_RG16_FLOAT = 34
    } EVertexFormat;

#ifdef __cplusplus
}
#endif

#endif