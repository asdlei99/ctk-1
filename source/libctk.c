
#if !defined(CTK_IMPLEMENTATION)
#include "libctk.h"
#endif

#include <stdio.h>  /* For sprintf(), etc. */
#include <assert.h>

#ifndef CTK_MALLOC
#define CTK_MALLOC(sz) malloc((sz))
#endif
#ifndef CTK_CALLOC
#define CTK_CALLOC(count, sz) calloc((count), (sz))
#endif
#ifndef CTK_REALLOC
#define CTK_REALLOC(p, sz) realloc((p), (sz))
#endif
#ifndef CTK_FREE
#define CTK_FREE(p) free((p))
#endif
#ifndef CTK_ZERO_MEMORY
#define CTK_ZERO_MEMORY(p, sz) memset((p), 0, (sz))
#endif
#ifndef CTK_COPY_MEMORY
#define CTK_COPY_MEMORY(dst, src, sz) memcpy((dst), (src), (sz))
#endif
#ifndef CTK_ASSERT
#define CTK_ASSERT(condition) assert(condition)
#endif

#define ctk_zero_memory CTK_ZERO_MEMORY
#define ctk_copy_memory CTK_COPY_MEMORY
#define ctk_assert      CTK_ASSERT

void* ctk_malloc(size_t sz)
{
    return CTK_MALLOC(sz);
}

void* ctk_calloc(size_t count, size_t sz)
{
    return CTK_CALLOC(count, sz);
}

void* ctk_realloc(void* p, size_t sz)
{
    return CTK_REALLOC(p, sz);
}

void ctk_free(void* p)
{
    CTK_FREE(p);
}

void* ctk_aligned_malloc(size_t sz, size_t alignment)
{
    size_t extraBytes;
    void* pUnaligned;
    void* pAligned;

    if (alignment == 0) {
        return 0;
    }

    extraBytes = alignment-1 + sizeof(void*);

    pUnaligned = ctk_malloc(sz + extraBytes);
    if (pUnaligned == NULL) {
        return NULL;
    }

    pAligned = (void*)(((ctk_uintptr)pUnaligned + extraBytes) & ~((ctk_uintptr)(alignment-1)));
    ((void**)pAligned)[-1] = pUnaligned;

    return pAligned;
}

void ctk_aligned_free(void* p)
{
    ctk_free(((void**)p)[-1]);
}


/* We use stb_printf, but we need to disable a few warnings with GCC. */
#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wlong-long"
    #pragma GCC diagnostic ignored "-Wunused-variable"
    #pragma GCC diagnostic ignored "-Wunused-function"
    #pragma GCC diagnostic ignored "-Wendif-labels"
    #pragma GCC diagnostic ignored "-Wreturn-type"
    #if defined(__clang__)
        #pragma GCC diagnostic ignored "-Wlanguage-extension-token"
    #endif
#endif
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif



#include "ctk_string.c"
#include "ctk_cmdline.c"

/**************************************************************************************************************************************************************
 **************************************************************************************************************************************************************

 Code Generation

 **************************************************************************************************************************************************************
 **************************************************************************************************************************************************************/
#include "ctk_codegen.c"

/**************************************************************************************************************************************************************
 **************************************************************************************************************************************************************

 Frontends

 **************************************************************************************************************************************************************
 **************************************************************************************************************************************************************/
#include "ctk_frontends.c"
