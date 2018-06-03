
typedef enum
{
    ctk_seek_origin_start,
    ctk_seek_origin_current
} ctk_seek_origin;


/* Wrapper API for fopen() for cleanly compiling against supported compilers. */
ctk_result ctk_fopen(const char* filePath, const char* openMode, FILE** ppFile);

/* Helper for creating an empty file. */
ctk_result ctk_create_empty_file(const char* fileName, ctk_bool32 failIfExists);

/* Retrieves the file data of the given file. Free the returned pointer with ctk_free(). */
ctk_result ctk_open_and_read_file(const char* filePath, size_t* pFileSizeOut, void** ppFileData);

/*
Retrieves the file data of the given file as a null terminated string. Free the returned pointer with ctk_free(). The
returned file size is the length of the string not including the null terminator.
*/
ctk_result ctk_open_and_read_text_file(const char* filePath, size_t* pFileSizeOut, char** ppFileData);

/* Creates a new file with the given data. */
ctk_result ctk_open_and_write_file(const char* filePath, const void* pData, size_t dataSize);

/* Creates a new file with the given string. */
ctk_result ctk_open_and_write_text_file(const char* filePath, const char* text);

/* Checks if the given path is an existing directory. */
ctk_bool32 ctk_is_directory(const char* path);

/*
Determines whether or not the given file path is to a file.

This will return CTK_FALSE if the path points to a directory.
*/
ctk_bool32 ctk_file_exists(const char* filePath);

/*
Determines whether or not the given file path points to a directory.

This will return CTK_FALSE if the path points to a file.
*/
CTK_INLINE ctk_bool32 ctk_directory_exists(const char* directoryPath) { return ctk_is_directory(directoryPath); }

/*
Moves a file.

This uses rename() on POSIX platforms and MoveFileEx(oldPath, newPath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH) on windows platforms.
*/
ctk_result ctk_move_file(const char* oldPath, const char* newPath);

/* Copies a file. */
ctk_result ctk_copy_file(const char* srcPath, const char* dstPath, ctk_bool32 failIfExists);

/* Determines if the given file is read only. */
ctk_bool32 ctk_is_file_read_only(const char* filePath);

/* Retrieves the last modified time of the file at the given path. */
ctk_uint64 ctk_get_file_modified_time(const char* filePath);

/*
Deletes the file at the given path.

This uses remove() on POSIX platforms and DeleteFile() on Windows platforms.
*/
ctk_result ctk_delete_file(const char* filePath);

/* Cross-platform wrapper for creating a directory. */
ctk_result ctk_mkdir(const char* directoryPath);

/* Recursively creates a directory. */
ctk_result ctk_mkdir_recursive(const char* directoryPath);


/* Callback function for file iteration. */
typedef ctk_bool32 (* ctk_iterate_files_proc)(const char* filePath, void* pUserData);

/* Iterates over every file and folder of the given directory. */
ctk_bool32 ctk_iterate_files(const char* directory, ctk_bool32 recursive, ctk_iterate_files_proc proc, void* pUserData);


/* Retrieves the current directory. Free the returned string with ctk_free(). Returns NULL on error. */
char* ctk_get_current_directory();

/* Sets the current directory. */
ctk_result ctk_set_current_directory(const char* path);
