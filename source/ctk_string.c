
ctk_bool32 ctk_string_is_null_or_empty(const char* str)
{
    return str == NULL || str[0] == '\0';
}



char* ctk_strcpy(char* dst, const char* src)
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

int ctk_strcpy_s(char* dst, size_t dstSizeInBytes, const char* src)
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

int ctk_strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
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

int ctk_strcat_s(char* dst, size_t dstSizeInBytes, const char* src)
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

int ctk_strncat_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
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

int ctk_itoa_s(int value, char* dst, size_t dstSizeInBytes, int radix)
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

size_t ctk_strcpy_len(char* dst, size_t dstSize, const char* src)
{
    if (ctk_strcpy_s(dst, dstSize, src) == 0) {
        return strlen(dst);
    }

    return 0;
}

int ctk_strcmp(const char* str1, const char* str2)
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

int ctk_stricmp(const char* str1, const char* str2)
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




ctk_uint32 ctk_utf16_to_utf32_ch(ctk_uint16 utf16[2])
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

ctk_uint32 ctk_utf16pair_to_utf32_ch(ctk_uint16 utf160, ctk_uint16 utf161)
{
    ctk_uint16 utf16[2];
    utf16[0] = utf160;
    utf16[1] = utf161;
    return ctk_utf16_to_utf32_ch(utf16);
}

ctk_uint32 ctk_utf32_to_utf16_ch(ctk_uint32 utf32, ctk_uint16 utf16[2])
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
ctk_uint32 ctk_utf32_to_utf8_ch(ctk_uint32 utf32, char* utf8, size_t utf8Size)
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

ctk_bool32 ctk_is_whitespace(ctk_uint32 utf32)
{
    return utf32 == ' ' || utf32 == '\t' || utf32 == '\n' || utf32 == '\v' || utf32 == '\f' || utf32 == '\r';
}

const char* ctk_first_non_whitespace(const char* str)
{
    if (str == NULL) {
        return NULL;
    }

    while (str[0] != '\0' && !(str[0] != ' ' && str[0] != '\t' && str[0] != '\n' && str[0] != '\v' && str[0] != '\f' && str[0] != '\r')) {
        str += 1;
    }

    return str;
}

const char* ctk_first_whitespace(const char* str)
{
    if (str == NULL) {
        return NULL;
    }

    while (str[0] != '\0' && (str[0] != ' ' && str[0] != '\t' && str[0] != '\n' && str[0] != '\v' && str[0] != '\f' && str[0] != '\r')) {
        str += 1;
    }

    return str;
}

const char* ctk_ltrim(const char* str)
{
    if (str == NULL) {
        return NULL;
    }

    while (str[0] != '\0' && !(str[0] != ' ' && str[0] != '\t' && str[0] != '\n' && str[0] != '\v' && str[0] != '\f' && str[0] != '\r')) {
        str += 1;
    }

    return str;
}

const char* ctk_rtrim(const char* str)
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

void ctk_trim(char* str)
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






ctk_string ctk_malloc_string(size_t sizeInBytesIncludingNullTerminator)
{
    if (sizeInBytesIncludingNullTerminator == 0) {
        return NULL;
    }

    return (ctk_string)ctk_calloc(sizeInBytesIncludingNullTerminator, 1);   /* Use calloc() to ensure it's null terminated. */
}

ctk_string ctk_make_string(const char* str)
{
    size_t len;
    char* newStr;

    if (str == NULL) return NULL;
    
    len = strlen(str);
    newStr = (char*)ctk_malloc(len+1);
    if (newStr == NULL) {
        return NULL;    /* Out of memory. */
    }

    ctk_strcpy_s(newStr, len+1, str);
    return newStr;
}

ctk_string ctk_make_stringv(const char* format, va_list args)
{
    va_list args2;
    char* str;
    int len;

    if (format == NULL) format = "";

    va_copy(args2, args);

    len = stbsp_vsnprintf(NULL, 0, format, args2);

    va_end(args2);
    if (len < 0) {
        return NULL;
    }


    str = (char*)ctk_malloc(len+1);
    if (str == NULL) {
        return NULL;
    }

    stbsp_vsnprintf(str, len+1, format, args);

    return str;
}

ctk_string ctk_make_stringf(const char* format, ...)
{
    va_list args;
    char* str;

    if (format == NULL) format = "";

    va_start(args, format);

    str = ctk_make_stringv(format, args);

    va_end(args);
    return str;
}

ctk_string ctk_make_string_length(const char* str, size_t strLen)
{
    char* newStr;

    if (str == NULL) return NULL;
    
    newStr = (char*)ctk_malloc(strLen+1);
    if (newStr == NULL) {
        return NULL;    /* Out of memory. */
    }

    ctk_strncpy_s(newStr, strLen+1, str, strLen);
    return newStr;
}

ctk_string ctk_set_string(ctk_string str, const char* newStr)
{
    if (newStr == NULL) newStr = "";

    if (str == NULL) {
        return ctk_make_string(newStr);
    } else {
        /* If there's enough room for the new string don't bother reallocating. */
        size_t oldStrCap = ctk_string_capacity(str);
        size_t newStrLen = strlen(newStr);

        if (oldStrCap < newStrLen) {
            str = (ctk_string)ctk_realloc(str, newStrLen + 1);  /* +1 for null terminator. */
            if (str == NULL) {
                return NULL;    /* Out of memory. */
            }
        }

        memcpy(str, newStr, newStrLen+1);   /* +1 to include the null terminator. */
        return str;
    }
}

ctk_string ctk_append_string(ctk_string lstr, const char* rstr)
{
    size_t lstrLen;
    size_t rstrLen;
    char* str;

    if (rstr == NULL) {
        rstr = "";
    }

    if (lstr == NULL) {
        return ctk_make_string(rstr);
    }

    lstrLen = ctk_string_length(lstr);
    rstrLen = strlen(rstr);
    str = (char*)ctk_realloc(lstr, lstrLen + rstrLen + 1);
    if (str == NULL) {
        return NULL;
    }

    memcpy(str + lstrLen, rstr, rstrLen);
    str[lstrLen + rstrLen] = '\0';

    return str;
}

ctk_string ctk_append_stringv(ctk_string lstr, const char* format, va_list args)
{
    char* str;

    ctk_string rstr = ctk_make_stringv(format, args);
    if (rstr == NULL) {
        return NULL;    /* Probably out of memory. */
    }

    str = ctk_append_string(lstr, rstr);

    ctk_free_string(rstr);
    return str;
}

ctk_string ctk_append_stringf(ctk_string lstr, const char* format, ...)
{
    char* str;

    va_list args;
    va_start(args, format);

    str = ctk_append_stringv(lstr, format, args);

    va_end(args);
    return str;
}

ctk_string ctk_append_string_length(ctk_string lstr, const char* rstr, size_t rstrLen)
{
    size_t lstrLen;
    char* str;

    if (rstr == NULL) {
        rstr = "";
    }

    if (lstr == NULL) {
        return ctk_make_string(rstr);
    }

    lstrLen = ctk_string_length(lstr);
    str = (char*)ctk_realloc(lstr, lstrLen + rstrLen + 1);
    if (str == NULL) {
        return NULL;
    }

    ctk_strncat_s(str, lstrLen + rstrLen + 1, rstr, rstrLen);
    str[lstrLen + rstrLen] = '\0';

    return str;
}

size_t ctk_string_length(ctk_string str)
{
    return strlen(str);
}

size_t ctk_string_capacity(ctk_string str)
{
    /*
    Currently we're not doing anything fancy with the memory management of strings, but this API is used right now
    so that future optimizations are easily enabled.
    */
    return ctk_string_length(str);
}

void ctk_free_string(ctk_string str)
{
    ctk_free(str);
}
