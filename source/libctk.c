
#if !defined(CTK_IMPLEMENTATION)
#include "libctk.h"
#endif

#if defined(CTK_WIN32)
    #if defined(_MSC_VER) && !defined(__clang__)
        #pragma warning(push)
        #pragma warning(disable:4115)
    #endif
    #include <windows.h>
    #if defined(_MSC_VER) && !defined(__clang__)
        #pragma warning(pop)
    #endif
#endif

#include <stdio.h>  /* For sprintf(), etc. */
#include <assert.h>

/* I couldn't figure out where SIZE_MAX was defined for VC6. If anybody knows, let me know. */
#if defined(_MSC_VER) && _MSC_VER <= 1200
#if defined(_WIN64)
#define SIZE_MAX    ((ctk_uint64)0xFFFFFFFFFFFFFFFF)
#else
#define SIZE_MAX    0xFFFFFFFF
#endif
#endif

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

#define ctk_zero_object(p)  ctk_zero_memory((p), sizeof(*(p)))
#define ctk_zero_memory     CTK_ZERO_MEMORY
#define ctk_copy_memory     CTK_COPY_MEMORY
#define ctk_assert          CTK_ASSERT

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


#if defined(CTK_WIN32)
ctk_result ctk_win32_error_to_result(DWORD error)
{
    switch (error)
    {
        case ERROR_SUCCESS:             return CTK_SUCCESS;
        case ERROR_PATH_NOT_FOUND:      return CTK_DOES_NOT_EXIST;
        case ERROR_TOO_MANY_OPEN_FILES: return CTK_TOO_MANY_OPEN_FILES;
        case ERROR_NOT_ENOUGH_MEMORY:   return CTK_OUT_OF_MEMORY;
        case ERROR_DISK_FULL:           return CTK_NO_SPACE;
        case ERROR_HANDLE_EOF:          return CTK_END_OF_FILE;
        case ERROR_NEGATIVE_SEEK:       return CTK_NEGATIVE_SEEK;
        case ERROR_INVALID_PARAMETER:   return CTK_INVALID_ARGS;
        case ERROR_ACCESS_DENIED:       return CTK_ACCESS_DENIED;
        case ERROR_SEM_TIMEOUT:         return CTK_TIMEOUT;
        case ERROR_FILE_NOT_FOUND:      return CTK_DOES_NOT_EXIST;
        default: break;
    }

    return CTK_ERROR;
}
#endif



/* We use stb_printf, but we need to disable a few warnings. */
#if defined(_MSC_VER) && !defined(__clang__)
    #pragma warning(push)
    #pragma warning(disable:4244)
    #pragma warning(disable:4100)
    #pragma warning(disable:4115)
#elif defined(__GNUC__) || defined(__clang__)
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
#if defined(_MSC_VER) && !defined(__clang__)
    #pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif



#include "ctk_string.c"
#include "ctk_cmdline.c"
#include "ctk_io.c"
#include "ctk_elf.c"
#include "ctk_coff.c"
#include "ctk_pe.c"

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
