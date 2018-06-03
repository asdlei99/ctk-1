
#ifndef _MSC_VER
    #ifndef _TRUNCATE
    #define _TRUNCATE ((size_t)-1)
    #endif
#endif

/* Determines whether or not the given string is null or empty. */
ctk_bool32 ctk_string_is_null_or_empty(const char* str);

/* strcpy() */
char* ctk_strcpy(char* dst, const char* src);
int ctk_strcpy_s(char* dst, size_t dstSizeInBytes, const char* src);
int ctk_strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count);
size_t ctk_strcpy_len(char* dst, size_t dstSize, const char* src);

/* strcat() */
int ctk_strcat_s(char* dst, size_t dstSizeInBytes, const char* src);
int ctk_strncat_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count);

/* itoa() */
int ctk_itoa_s(int value, char* dst, size_t dstSizeInBytes, int radix);

/* strcmp() */
int ctk_strcmp(const char* str1, const char* str2);
int ctk_stricmp(const char* str1, const char* str2);



/* Converts a UTF-16 character to UTF-32. */
ctk_uint32 ctk_utf16_to_utf32_ch(ctk_uint16 utf16[2]);

/* Converts a UTF-16 surrogate pair to UTF-32. */
ctk_uint32 ctk_utf16pair_to_utf32_ch(ctk_uint16 utf160, ctk_uint16 utf161);

/* Converts a UTF-32 character to a UTF-16. Returns the number fo UTF-16 values making up the character. */
ctk_uint32 ctk_utf32_to_utf16_ch(ctk_uint32 utf32, ctk_uint16 utf16[2]);

/* Converts a UTF-32 character to a UTF-8 character. Returns the number of bytes making up the UTF-8 character. */
ctk_uint32 ctk_utf32_to_utf8_ch(ctk_uint32 utf32, char* utf8, size_t utf8Size);

/* Whitespace */
ctk_bool32 ctk_is_whitespace(ctk_uint32 utf32);
const char* ctk_first_non_whitespace(const char* str);
const char* ctk_first_whitespace(const char* str);

/* Trimming */
const char* ctk_ltrim(const char* str);
const char* ctk_rtrim(const char* str);
void ctk_trim(char* str);



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
