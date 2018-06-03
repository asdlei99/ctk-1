/*
Compiler toolkit. Public domain. See "unlicense" statement at the end of this file.
ctk - v0.x - 2018-xx-xx

David Reid - davidreidsoftware@gmail.com
*/
#ifndef libctk_h
#define libctk_h

#ifdef __cplusplus
extern "C" {
#endif

#define CTK_STRINGIFY(x)        #x
#define CTK_XSTRINGIFY(x)       CTK_STRINGIFY(x)

#define CTK_VERSION_MAJOR       0
#define CTK_VERSION_MINOR       0
#define CTK_VERSION_REVISION    0
#define CTK_VERSION             CTK_XSTRINGIFY(CTK_VERSION_MAJOR) "." CTK_XSTRINGIFY(CTK_VERSION_MINOR) "." CTK_XSTRINGIFY(CTK_VERSION_REVISION)

#define CTK_UNUSED(x)           ((void)(x))

#if defined(__GNUC__) || defined(__clang__)
#define CTK_INLINE              static __inline__ __attribute__((always_inline))
#elif defined(_MSC_VER) && !defined(__clang__)
#define CTK_INLINE              static __forceinline
#else
#define CTK_INLINE
#endif

#include <stddef.h> /* For size_t */
#include <errno.h>  /* For EINVAL */

#ifndef CTK_HAS_STDINT
    #if defined(_MSC_VER)
        #if _MSC_VER >= 1600
            #define CTK_HAS_STDINT
        #endif
    #else
        #if defined(__has_include)
            #if __has_include(<stdint.h>)
                #define CTK_HAS_STDINT
            #endif
        #endif
    #endif
#endif

#ifndef CTK_HAS_STDINT
typedef   signed char               ctk_int8;
typedef unsigned char               ctk_uint8;
typedef   signed short              ctk_int16;
typedef unsigned short              ctk_uint16;
typedef   signed int                ctk_int32;
typedef unsigned int                ctk_uint32;
    #if defined(_MSC_VER)
    typedef   signed __int64        ctk_int64;
    typedef unsigned __int64        ctk_uint64;
    #else
    typedef   signed long long int  ctk_int64;
    typedef unsigned long long int  ctk_uint64;
    #endif
    #if defined(_WIN32)
        #if defined(_WIN64)
        typedef ctk_uint64          ctk_uintptr;
        #else
        typedef ctk_uint32          ctk_uintptr;
        #endif
    #elif defined(__GNUC__)
        #if defined(__LP64__)
        typedef ctk_uint64          ctk_uintptr;
        #else
        typedef ctk_uint32          ctk_uintptr;
        #endif
    #else
        typedef ctk_uint64          ctk_uintptr;    /* Fallback. */
    #endif
#else
#include <stdint.h>
typedef int8_t                      ctk_int8;
typedef uint8_t                     ctk_uint8;
typedef int16_t                     ctk_int16;
typedef uint16_t                    ctk_uint16;
typedef int32_t                     ctk_int32;
typedef uint32_t                    ctk_uint32;
typedef int64_t                     ctk_int64;
typedef uint64_t                    ctk_uint64;
typedef uintptr_t                   ctk_uintptr;
#endif
typedef ctk_uint8                   ctk_bool8;
typedef ctk_uint32                  ctk_bool32;
#define CTK_TRUE                    1
#define CTK_FALSE                   0

#include <string.h> /* For strcpy() */
#include <stdarg.h> /* For va_list */
#include <ctype.h>  /* For toupper() */
#include <stdlib.h>

#if defined(__GNUC__)
    #ifndef va_copy
    #define va_copy(dst, src) __builtin_va_copy((dst), (src))
    #endif
#endif
#if defined(_MSC_VER) && !defined(__clang__)
    #ifndef va_copy
    #define va_copy(dst, src) ((dst) = (src))
    #endif
#endif

void* ctk_malloc(size_t sz);
void* ctk_calloc(size_t count, size_t sz);
void* ctk_realloc(void* p, size_t sz);
void  ctk_free(void* p);
void* ctk_aligned_malloc(size_t sz, size_t alignment);
void  ctk_aligned_free(void* p);


#include "ctk_string.h"
#include "ctk_cmdline.h"

/**************************************************************************************************************************************************************
 **************************************************************************************************************************************************************

 Code Generation

 **************************************************************************************************************************************************************
 **************************************************************************************************************************************************************/
#include "ctk_codegen.h"

/**************************************************************************************************************************************************************
 **************************************************************************************************************************************************************

 Frontends

 **************************************************************************************************************************************************************
 **************************************************************************************************************************************************************/
#include "ctk_frontends.h"


#ifdef __cplusplus
}
#endif
#endif  /* libctk_h */

#if defined(CTK_IMPLEMENTATION)
#include "libctk.c"
#endif

/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/
