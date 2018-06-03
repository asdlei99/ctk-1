
/* Determines whether or not the given string is null or empty. */
CTK_INLINE ctk_bool32 ctk_string_is_null_or_empty(const char* str)
{
    return str == NULL || str[0] == '\0';
}

#ifndef _MSC_VER
    #ifndef _TRUNCATE
    #define _TRUNCATE ((size_t)-1)
    #endif
#endif

CTK_INLINE char* ctk_strcpy(char* dst, const char* src)
{
    if (dst == NULL) return NULL;

    /* If the source string is null, just pretend it's an empty string. I don't believe this is standard behaviour of strcpy(), but I prefer it. */
    if (src == NULL) {
        src = "\0";
    }

#if defined(_MSC_VER) && _MSC_VER >= 1500
    while ((*dst++ = *src++));
    return dst;
#else
    return strcpy(dst, src);
#endif
}

CTK_INLINE int ctk_strcpy_s(char* dst, size_t dstSizeInBytes, const char* src)
{
#if defined(_MSC_VER) && _MSC_VER >= 1500
    return strcpy_s(dst, dstSizeInBytes, src);
#else
    size_t i;

    if (dst == 0) {
        return EINVAL;
    }
    if (dstSizeInBytes == 0) {
        return ERANGE;
    }
    if (src == 0) {
        dst[0] = '\0';
        return EINVAL;
    }

    for (i = 0; i < dstSizeInBytes && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (i < dstSizeInBytes) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return ERANGE;
#endif
}

CTK_INLINE int ctk_strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
#if defined(_MSC_VER) && _MSC_VER >= 1500
    return strncpy_s(dst, dstSizeInBytes, src, count);
#else
    size_t maxcount;
    size_t i;

    if (dst == 0) {
        return EINVAL;
    }
    if (dstSizeInBytes == 0) {
        return EINVAL;
    }
    if (src == 0) {
        dst[0] = '\0';
        return EINVAL;
    }

    maxcount = count;
    if (count == ((size_t)-1) || count >= dstSizeInBytes) {        /* -1 = _TRUNCATE */
        maxcount = dstSizeInBytes - 1;
    }

    for (i = 0; i < maxcount && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (src[i] == '\0' || i == count || count == ((size_t)-1)) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return ERANGE;
#endif
}

CTK_INLINE int ctk_strcat_s(char* dst, size_t dstSizeInBytes, const char* src)
{
#if defined(_MSC_VER) && _MSC_VER >= 1500
    return strcat_s(dst, dstSizeInBytes, src);
#else
    char* dstorig;

    if (dst == 0) {
        return EINVAL;
    }
    if (dstSizeInBytes == 0) {
        return ERANGE;
    }
    if (src == 0) {
        dst[0] = '\0';
        return EINVAL;
    }

    dstorig = dst;

    while (dstSizeInBytes > 0 && dst[0] != '\0') {
        dst += 1;
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes == 0) {
        return EINVAL;  /* Unterminated. */
    }


    while (dstSizeInBytes > 0 && src[0] != '\0') {
        *dst++ = *src++;
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes > 0) {
        dst[0] = '\0';
    } else {
        dstorig[0] = '\0';
        return ERANGE;
    }

    return 0;
#endif
}

CTK_INLINE int ctk_strncat_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
#if defined(_MSC_VER) && _MSC_VER >= 1500
    return strncat_s(dst, dstSizeInBytes, src, count);
#else
    char* dstorig = dst;

    if (dst == 0) {
        return EINVAL;
    }
    if (dstSizeInBytes == 0) {
        return ERANGE;
    }
    if (src == 0) {
        return EINVAL;
    }

    dstorig = dst;

    while (dstSizeInBytes > 0 && dst[0] != '\0') {
        dst += 1;
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes == 0) {
        return EINVAL;  /* Unterminated. */
    }


    if (count == ((size_t)-1)) {        /* _TRUNCATE */
        count = dstSizeInBytes - 1;
    }

    while (dstSizeInBytes > 0 && src[0] != '\0' && count > 0)
    {
        *dst++ = *src++;
        dstSizeInBytes -= 1;
        count -= 1;
    }

    if (dstSizeInBytes > 0) {
        dst[0] = '\0';
    } else {
        dstorig[0] = '\0';
        return ERANGE;
    }

    return 0;
#endif
}

CTK_INLINE int ctk_itoa_s(int value, char* dst, size_t dstSizeInBytes, int radix)
{
#if defined(_MSC_VER) && _MSC_VER >= 1500
    return _itoa_s(value, dst, dstSizeInBytes, radix);
#else
    int sign;
    unsigned int valueU;
    char* dstEnd;

    if (dst == NULL || dstSizeInBytes == 0) {
        return EINVAL;
    }
    if (radix < 2 || radix > 36) {
        dst[0] = '\0';
        return EINVAL;
    }

    sign = (value < 0 && radix == 10) ? -1 : 1;     /* The negative sign is only used when the base is 10. */

    if (value < 0) {
        valueU = -value;
    } else {
        valueU = value;
    }

    dstEnd = dst;
    do
    {
        int remainder = valueU % radix;
        if (remainder > 9) {
            *dstEnd = (char)((remainder - 10) + 'a');
        } else {
            *dstEnd = (char)(remainder + '0');
        }

        dstEnd += 1;
        dstSizeInBytes -= 1;
        valueU /= radix;
    } while (dstSizeInBytes > 0 && valueU > 0);

    if (dstSizeInBytes == 0) {
        dst[0] = '\0';
        return EINVAL;  /* Ran out of room in the output buffer. */
    }

    if (sign < 0) {
        *dstEnd++ = '-';
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes == 0) {
        dst[0] = '\0';
        return EINVAL;  /* Ran out of room in the output buffer. */
    }

    *dstEnd = '\0';


    /* At this point the string will be reversed. */
    dstEnd -= 1;
    while (dst < dstEnd) {
        char temp = *dst;
        *dst = *dstEnd;
        *dstEnd = temp;

        dst += 1;
        dstEnd -= 1;
    }

    return 0;
#endif
}

CTK_INLINE size_t ctk_strcpy_len(char* dst, size_t dstSize, const char* src)
{
    if (ctk_strcpy_s(dst, dstSize, src) == 0) {
        return strlen(dst);
    }

    return 0;
}

CTK_INLINE int mal_strcmp(const char* str1, const char* str2)
{
    if (str1 == str2) return  0;

    /*
    These checks differ from the standard implementation. It's not important, but I prefer
    it just for sanity.
    */
    if (str1 == NULL) return -1;
    if (str2 == NULL) return  1;

    for (;;) {
        if (str1[0] == '\0') {
            break;
        }
        if (str1[0] != str2[0]) {
            break;
        }

        str1 += 1;
        str2 += 1;
    }

    return ((unsigned char*)str1)[0] - ((unsigned char*)str2)[0];
}

CTK_INLINE int ctk_stricmp(const char* str1, const char* str2)
{
#if defined(__GNUC__) && defined(__STRICT_ANSI__)
    int upper1;
    int upper2;

    if (str1 == str2) return  0;
    if (str1 == NULL) return -1;
    if (str2 == NULL) return  1;

    for (;;) {
        upper1 = toupper(str1[0]);
        upper2 = toupper(str2[0]);

        if (upper1 == '\0') {
            break;
        }

        if (upper1 != upper2) {
            break;
        }

        str1 += 1;
        str2 += 1;
    }

    return upper1 - upper2;
#else
    #if defined(_MSC_VER) || defined(__MINGW32__)
        return _stricmp(str1, str2);
    #else
        return strcasecmp(str1, str2);
    #endif
#endif
}




#ifndef _MSC_VER
CTK_INLINE int strcpy_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    return ctk_strcpy_s(dst, dstSizeInBytes, src);
}

CTK_INLINE int strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
    return ctk_strncpy_s(dst, dstSizeInBytes, src, count);
}

CTK_INLINE int strcat_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    return ctk_strcat_s(dst, dstSizeInBytes, src);
}

CTK_INLINE int strncat_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
    return ctk_strncat_s(dst, dstSizeInBytes, src, count);
}

#ifndef __MINGW32__
CTK_INLINE int _stricmp(const char* string1, const char* string2)
{
    return strcasecmp(string1, string2);
}
#endif

CTK_INLINE int _itoa_s(int value, char* dst, size_t dstSizeInBytes, int radix)
{
    return ctk_itoa_s(value, dst, dstSizeInBytes, radix);
}
#endif


/* Converts a UTF-16 character to UTF-32. */
CTK_INLINE ctk_uint32 ctk_utf16_to_utf32_ch(ctk_uint16 utf16[2])
{
    if (utf16 == NULL) {
        return 0;
    }

    if (utf16[0] < 0xD800 || utf16[0] > 0xDFFF) {
        return utf16[0];
    } else {
        if ((utf16[0] & 0xFC00) == 0xD800 && (utf16[1] & 0xFC00) == 0xDC00) {
            return ((ctk_uint32)utf16[0] << 10) + utf16[1] - 0x35FDC00;
        } else {
            return 0;   /* Invalid. */
        }
    }
}

/* Converts a UTF-16 surrogate pair to UTF-32. */
CTK_INLINE ctk_uint32 ctk_utf16pair_to_utf32_ch(ctk_uint16 utf160, ctk_uint16 utf161)
{
    ctk_uint16 utf16[2];
    utf16[0] = utf160;
    utf16[1] = utf161;
    return ctk_utf16_to_utf32_ch(utf16);
}

/* Converts a UTF-32 character to a UTF-16. Returns the number fo UTF-16 values making up the character. */
CTK_INLINE ctk_uint32 ctk_utf32_to_utf16_ch(ctk_uint32 utf32, ctk_uint16 utf16[2])
{
    if (utf16 == NULL) {
        return 0;
    }

    if (utf32 < 0xD800 || (utf32 >= 0xE000 && utf32 <= 0xFFFF)) {
        utf16[0] = (ctk_uint16)utf32;
        utf16[1] = 0;
        return 1;
    } else {
        if (utf32 >= 0x10000 && utf32 <= 0x10FFFF) {
            utf16[0] = (ctk_uint16)(0xD7C0 + (ctk_uint16)(utf32 >> 10));
            utf16[1] = (ctk_uint16)(0xDC00 + (ctk_uint16)(utf32 & 0x3FF));
            return 2;
        } else {
            /* Invalid. */
            utf16[0] = 0;
            utf16[1] = 0;
            return 0;
        }
    }
}

/* Converts a UTF-32 character to a UTF-8 character. Returns the number of bytes making up the UTF-8 character. */
CTK_INLINE ctk_uint32 ctk_utf32_to_utf8_ch(ctk_uint32 utf32, char* utf8, size_t utf8Size)
{
    const unsigned char firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};

    ctk_uint32 utf8ByteCount = 0;
    if (utf32 < 0x80) {
        utf8ByteCount = 1;
    } else if (utf32 < 0x800) {
        utf8ByteCount = 2;
    } else if (utf32 < 0x10000) {
        utf8ByteCount = 3;
    } else if (utf32 < 0x110000) {
        utf8ByteCount = 4;
    }

    if (utf8ByteCount > utf8Size) {
        if (utf8 != NULL && utf8Size > 0) {
            utf8[0] = '\0';
        }
        return 0;
    }

    utf8 += utf8ByteCount;
    if (utf8ByteCount < utf8Size) {
        utf8[0] = '\0'; /* Null terminate. */
    }

    switch (utf8ByteCount)
    {
        case 4: *--utf8 = (char)((utf32 | 0x80) & 0xBF); utf32 >>= 6;
        case 3: *--utf8 = (char)((utf32 | 0x80) & 0xBF); utf32 >>= 6;
        case 2: *--utf8 = (char)((utf32 | 0x80) & 0xBF); utf32 >>= 6;
        case 1: *--utf8 = (char)(utf32 | firstByteMark[utf8ByteCount]);
        default: break;
    }

    return utf8ByteCount;
}

CTK_INLINE ctk_bool32 ctk_is_whitespace(ctk_uint32 utf32)
{
    return utf32 == ' ' || utf32 == '\t' || utf32 == '\n' || utf32 == '\v' || utf32 == '\f' || utf32 == '\r';
}

CTK_INLINE const char* ctk_first_non_whitespace(const char* str)
{
    if (str == NULL) {
        return NULL;
    }

    while (str[0] != '\0' && !(str[0] != ' ' && str[0] != '\t' && str[0] != '\n' && str[0] != '\v' && str[0] != '\f' && str[0] != '\r')) {
        str += 1;
    }

    return str;
}

CTK_INLINE const char* ctk_first_whitespace(const char* str)
{
    if (str == NULL) {
        return NULL;
    }

    while (str[0] != '\0' && (str[0] != ' ' && str[0] != '\t' && str[0] != '\n' && str[0] != '\v' && str[0] != '\f' && str[0] != '\r')) {
        str += 1;
    }

    return str;
}

CTK_INLINE const char* ctk_ltrim(const char* str)
{
    if (str == NULL) {
        return NULL;
    }

    while (str[0] != '\0' && !(str[0] != ' ' && str[0] != '\t' && str[0] != '\n' && str[0] != '\v' && str[0] != '\f' && str[0] != '\r')) {
        str += 1;
    }

    return str;
}

CTK_INLINE const char* ctk_rtrim(const char* str)
{
    const char* rstr;

    if (str == NULL) {
        return NULL;
    }

    rstr = str;
    while (str[0] != '\0') {
        if (ctk_is_whitespace(str[0])) {
            str += 1;
            continue;
        }

        str += 1;
        rstr = str;
    }

    return rstr;
}

CTK_INLINE void ctk_trim(char* str)
{
    const char* lstr;
    const char* rstr;

    if (str == NULL) {
        return;
    }

    lstr = ctk_ltrim(str);
    rstr = ctk_rtrim(lstr);

    if (lstr > str) {
        memmove(str, lstr, rstr-lstr);
    }

    str[rstr-lstr] = '\0';
}


/**** ctk_string ****/

typedef char* ctk_string;

/*
Allocates the memory for a string, including the null terminator.

Use this API if you want to allocate memory for the string, but you want to fill it with raw data yourself.
*/
ctk_string ctk_malloc_string(size_t sizeInBytesIncludingNullTerminator);

/* Creates a newly allocated string. Free the string with ctk_free_string(). */
ctk_string ctk_make_string(const char* str);

/* Creates a formatted string. Free the string with ctk_free_string(). */
ctk_string ctk_make_stringv(const char* format, va_list args);
ctk_string ctk_make_stringf(const char* format, ...);

/* Creates a newly allocated string, restricting it to a maximum length. */
ctk_string ctk_make_string_length(const char* str, size_t strLen);

/* Changes the value of a string. */
ctk_string ctk_set_string(ctk_string str, const char* newStr);

/*
Appends a string to another ctk_string.

This free's "lstr". Use this API like so: "lstr = ctk_append_string(lstr, rstr)". It works the same way as realloc().

Use ctk_make_stringf("%s%s", str1, str2) to append to C-style strings together. An optimized solution for this may be implemented in the future.
*/
ctk_string ctk_append_string(ctk_string lstr, const char* rstr);

/* Appends a formatted string to another ctk_string. */
ctk_string ctk_append_stringv(ctk_string lstr, const char* format, va_list args);
ctk_string ctk_append_stringf(ctk_string lstr, const char* format, ...);

/*
Same as ctk_append_string(), except restricts it to a maximum number of characters and does not require the input
string to be null terminated.
*/
ctk_string ctk_append_string_length(ctk_string lstr, const char* rstr, size_t rstrLen);

/* Retrieves the length of the given string. */
size_t ctk_string_length(ctk_string str);

/*
Retrieves the capacity of the buffer containing the data of the given string, not including the null terminator. Add 1
to the returned value to get the size of the entire buffer.
*/
size_t ctk_string_capacity(ctk_string str);


/* Frees a string created by ctk_make_string*() */
void ctk_free_string(ctk_string str);
