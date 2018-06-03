
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
