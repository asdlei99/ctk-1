
#ifdef CTK_WIN32

/* For VC6. */
#if !defined(INVALID_FILE_ATTRIBUTES)
#define INVALID_FILE_ATTRIBUTES     0xFFFFFFFF
#endif

ctk_bool32 ctk_is_directory__win32(const char* path)
{
    DWORD attributes;
    ctk_assert(path != NULL);

    attributes = GetFileAttributesA(path);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

ctk_bool32 ctk_file_exists__win32(const char* filePath)
{
    DWORD attributes;
    ctk_assert(filePath != NULL);

    attributes = GetFileAttributesA(filePath);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

char* ctk_get_current_directory__win32()
{
    char* pDir;

    DWORD len = GetCurrentDirectoryA(0, NULL);
    if (len == 0) {
        return NULL;
    }

    pDir = (char*)ctk_malloc(len);
    if (pDir == NULL) {
        return NULL;    /* Out of memory. */
    }

    len = GetCurrentDirectoryA(len, pDir);
    if (len == 0) {
        ctk_free(pDir);
        return NULL;
    }

    return pDir;
}

ctk_result ctk_set_current_directory__win32(const char* path)
{
    if (SetCurrentDirectoryA(path) == 0) {
        return CTK_ERROR;
    }

    return CTK_SUCCESS;
}
#endif


#ifdef CTK_POSIX
ctk_bool32 ctk_is_directory__posix(const char* path)
{
    struct stat info;
    ctk_assert(path != NULL);

    if (stat(path, &info) != 0) {
        return CTK_FALSE;   /* Likely the folder doesn't exist. */
    }

    return (info.st_mode & S_IFDIR) != 0;
}

ctk_bool32 ctk_file_exists__posix(const char* filePath)
{
    struct stat info;
    ctk_assert(filePath != NULL);

    if (stat(filePath, &info) != 0) {
        return CTK_FALSE;   /* Likely the folder doesn't exist. */
    }

    return (info.st_mode & S_IFDIR) == 0;
}

char* ctk_get_current_directory__posix()
{
    size_t len;
    char* pDir;

    char* pDirTemp = getcwd(NULL, 0);
    if (pDirTemp == NULL) {
        return NULL;
    }

    /*
    Unfortunately getcwd() explicitly uses malloc() for the allocation, however we require the caller
    to free with ctk_free(). To ensure correctness, we need to allocate it again :(
    */
    len = strlen(pDirTemp);
    pDir = (char*)ctk_malloc(len + 1);
    if (pDir == NULL) {
        return NULL;    /* Out of memory. */
    }

    ctk_strcpy(pDir, pDirTemp);
    free(pDirTemp);

    return pDir;
}

ctk_result ctk_set_current_directory__posix(const char* path)
{
    if (chdir(path) != 0) {
        return CTK_ERROR;
    }

    return CTK_SUCCESS;
}
#endif


ctk_result ctk_fopen(const char* filePath, const char* openMode, FILE** ppFile)
{
    if (filePath == NULL || openMode == NULL || ppFile == NULL) return CTK_INVALID_ARGS;

#if defined(_MSC_VER) && _MSC_VER > 1400   /* 1400 = Visual Studio 2005 */
    {
        if (fopen_s(ppFile, filePath, openMode) != 0) {
            return CTK_FAILED_TO_OPEN_FILE;
        }
    }
#else
    {
        FILE* pFile = fopen(filePath, openMode);
        if (pFile == NULL) {
            return CTK_FAILED_TO_OPEN_FILE;
        }

        *ppFile = pFile;
    }
#endif

    return CTK_SUCCESS;
}

ctk_result ctk_create_empty_file(const char* fileName, ctk_bool32 failIfExists)
{
    if (fileName == NULL) return CTK_INVALID_ARGS;

#ifdef _WIN32
    {
        HANDLE hFile;

        DWORD dwCreationDisposition;
        if (failIfExists) {
            dwCreationDisposition = CREATE_NEW;
        } else {
            dwCreationDisposition = CREATE_ALWAYS;
        }

        hFile = CreateFileA(fileName, FILE_GENERIC_WRITE, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            return ctk_win32_error_to_result(GetLastError());
        }

        CloseHandle(hFile);
        return CTK_SUCCESS;
    }
#else
    {
        int fd;

        int flags = O_WRONLY | O_CREAT;
        if (failIfExists) {
            flags |= O_EXCL;
        } else {
            flags |= O_TRUNC;
        }

        fd = open(fileName, flags, 0666);
        if (fd == -1) {
            return ctk_errno_to_result(errno);
        }

        close(fd);
        return CTK_SUCCESS;
    }
#endif
}

static ctk_result ctk_open_and_read_file_with_extra_data(const char* filePath, size_t* pFileSizeOut, void** ppFileData, size_t extraBytes)
{
    ctk_result result;
    FILE* pFile;
    ctk_uint64 fileSize;
    void* pFileData;
    size_t bytesRead;

    /* Safety. */
    if (pFileSizeOut) *pFileSizeOut = 0;
    if (ppFileData) *ppFileData = NULL;

    if (filePath == NULL) {
        return CTK_INVALID_ARGS;
    }

    /* TODO: Use 64-bit versions of the FILE APIs. */

    result = ctk_fopen(filePath, "rb", &pFile);
    if (result != CTK_SUCCESS) {
        return CTK_FAILED_TO_OPEN_FILE;
    }

    fseek(pFile, 0, SEEK_END);
    fileSize = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    if (fileSize + extraBytes > SIZE_MAX) {
        fclose(pFile);
        return CTK_FILE_TOO_BIG;
    }

    pFileData = ctk_malloc((size_t)fileSize + extraBytes);    /* <-- Safe cast due to the check above. */
    if (pFileData == NULL) {
        fclose(pFile);
        return CTK_OUT_OF_MEMORY;
    }

    bytesRead = fread(pFileData, 1, (size_t)fileSize, pFile);
    if (bytesRead != fileSize) {
        ctk_free(pFileData);
        fclose(pFile);
        return CTK_FAILED_TO_READ_FILE;
    }

    fclose(pFile);

    if (pFileSizeOut) {
        *pFileSizeOut = (size_t)fileSize;
    }

    if (ppFileData) {
        *ppFileData = pFileData;
    } else {
        ctk_free(pFileData);
    }

    return CTK_SUCCESS;
}

ctk_result ctk_open_and_read_file(const char* filePath, size_t* pFileSizeOut, void** ppFileData)
{
    return ctk_open_and_read_file_with_extra_data(filePath, pFileSizeOut, ppFileData, 0);
}

ctk_result ctk_open_and_read_text_file(const char* filePath, size_t* pFileSizeOut, char** ppFileData)
{
    size_t fileSize;
    char* pFileData;
    ctk_result result = ctk_open_and_read_file_with_extra_data(filePath, &fileSize, (void**)&pFileData, 1);
    if (result != CTK_SUCCESS) {
        return result;
    }

    pFileData[fileSize] = '\0';

    if (pFileSizeOut) {
        *pFileSizeOut = fileSize;
    }

    if (ppFileData) {
        *ppFileData = pFileData;
    } else {
        ctk_free(pFileData);
    }

    return CTK_SUCCESS;
}

ctk_result ctk_open_and_write_file(const char* filePath, const void* pData, size_t dataSize)
{
    ctk_result result;
    FILE* pFile;

    if (filePath == NULL) {
        return CTK_INVALID_ARGS;
    }

    /* TODO: Use 64-bit versions of the FILE APIs. */

    result = ctk_fopen(filePath, "wb", &pFile);
    if (result != CTK_SUCCESS) {
        return CTK_FAILED_TO_OPEN_FILE;
    }

    if (pData != NULL && dataSize > 0) {
        if (fwrite(pData, 1, dataSize, pFile) != dataSize) {
            fclose(pFile);
            return CTK_FAILED_TO_WRITE_FILE;
        }
    }

    fclose(pFile);
    return CTK_SUCCESS;
}

ctk_result ctk_open_and_write_text_file(const char* filePath, const char* text)
{
    if (text == NULL) {
        text = "";
    }

    return ctk_open_and_write_file(filePath, text, strlen(text));
}

ctk_bool32 ctk_is_directory(const char* path)
{
    if (ctk_string_is_null_or_empty(path)) return CTK_FALSE;

#ifdef CTK_WIN32
    return ctk_is_directory__win32(path);
#endif
#ifdef CTK_POSIX
    return ctk_is_directory__posix(path);
#endif
}

ctk_bool32 ctk_file_exists(const char* filePath)
{
    if (filePath == NULL) return CTK_FALSE;

#ifdef CTK_WIN32
    return ctk_file_exists__win32(filePath);
#endif
#ifdef CTK_POSIX
    return ctk_file_exists__posix(filePath);
#endif
}


ctk_result ctk_move_file(const char* oldPath, const char* newPath)
{
    if (oldPath == NULL || newPath == NULL) return CTK_INVALID_ARGS;

#if _WIN32
    if (MoveFileExA(oldPath, newPath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH) == 0) {
        return ctk_win32_error_to_result(GetLastError());
    }

    return CTK_SUCCESS;
#else
    if (rename(oldPath, newPath) != 0) {
        return CTK_ERROR;
    }

    return CTK_SUCCESS;
#endif
}

ctk_result ctk_copy_file(const char* srcPath, const char* dstPath, ctk_bool32 failIfExists)
{
    if (srcPath == NULL || dstPath == NULL) return CTK_INVALID_ARGS;

#if _WIN32
    if (CopyFileA(srcPath, dstPath, failIfExists) == 0) {
        return ctk_win32_error_to_result(GetLastError());
    }

    return CTK_SUCCESS;
#else
    ctk_bool32 result;
    struct stat info;

    int fdDst;
    int fdSrc = open(srcPath, O_RDONLY, 0666);
    if (fdSrc == -1) {
        return ctk_errno_to_result(errno);
    }

    fdDst = open(dstPath, O_WRONLY | O_TRUNC | O_CREAT | ((failIfExists) ? O_EXCL : 0), 0666);
    if (fdDst == -1) {
        close(fdSrc);
        return ctk_errno_to_result(errno);
    }

    result = CTK_TRUE;
    if (fstat(fdSrc, &info) == 0) {
        char buffer[BUFSIZ];
        int bytesRead;
        while ((bytesRead = read(fdSrc, buffer, sizeof(buffer))) > 0) {
            if (write(fdDst, buffer, bytesRead) != bytesRead) {
                result = CTK_FALSE;
                break;
            }
        }
    } else {
        result = CTK_FALSE;
    }

    close(fdDst);
    close(fdSrc);

    /* Permissions. */
    chmod(dstPath, info.st_mode & 07777);

    if (result == CTK_FALSE) {
        return ctk_errno_to_result(errno);
    }

    return CTK_SUCCESS;
#endif
}

ctk_bool32 ctk_is_file_read_only(const char* filePath)
{
    if (filePath == NULL || filePath[0] == '\0') return CTK_FALSE;

#if _WIN32
    {
        DWORD attributes = GetFileAttributesA(filePath);
        return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_READONLY) != 0;
    }
#else
    return access(filePath, W_OK) == -1;
#endif
}

ctk_uint64 ctk_get_file_modified_time(const char* filePath)
{
    if (filePath == NULL || filePath[0] == '\0') {
        return 0;
    }

#if _WIN32
    {
        FILETIME fileTime;
        BOOL wasSuccessful;
        ULARGE_INTEGER result;

        HANDLE hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            return 0;
        }

        wasSuccessful = GetFileTime(hFile, NULL, NULL, &fileTime);
        CloseHandle(hFile);

        if (!wasSuccessful) {
            return 0;
        }

        result.HighPart = fileTime.dwHighDateTime;
        result.LowPart = fileTime.dwLowDateTime;
        return result.QuadPart;
    }
#else
    {
        struct stat info;
        if (stat(filePath, &info) != 0) {
            return 0;
        }

        return info.st_mtime;
    }
#endif
}


ctk_result ctk_delete_file(const char* filePath)
{
    if (filePath == NULL) return CTK_INVALID_ARGS;

#ifdef CTK_WIN32
    if (DeleteFileA(filePath) == 0) {
        return ctk_win32_error_to_result(GetLastError());
    }

    return CTK_SUCCESS;
#endif
#ifdef CTK_POSIX
    if (remove(filePath) != 0) {
        return ctk_errno_to_result(errno);
    }

    return CTK_SUCCESS;
#endif
}

ctk_result ctk_mkdir(const char* directoryPath)
{
    if (directoryPath == NULL) return CTK_INVALID_ARGS;

#ifdef CTK_WIN32
    if (CreateDirectoryA(directoryPath, NULL) == 0) {
        return ctk_win32_error_to_result(GetLastError());
    }

    return CTK_SUCCESS;
#endif
#ifdef CTK_POSIX
    if (mkdir(directoryPath, 0777) != 0) {
        return ctk_errno_to_result(errno);
    }

    return CTK_SUCCESS;
#endif
}

ctk_result ctk_mkdir_recursive(const char* directoryPath)
{
    /* TODO: Don't restrict this to 4096 characters. */
    char runningPath[4096];
    size_t i;

    if (directoryPath == NULL || directoryPath[0] == '\0') {
        return CTK_INVALID_ARGS;
    }

    /* All we need to do is iterate over every segment in the path and try creating the directory. */
    ctk_zero_memory(runningPath, sizeof(runningPath));

    i = 0;
    for (;;) {
        if (i >= sizeof(runningPath)-1) {
            return CTK_PATH_TOO_LONG;   /* Path is too long. */
        }

        if (directoryPath[0] == '\0' || directoryPath[0] == '/' || directoryPath[0] == '\\') {
            if (runningPath[0] != '\0' && !(runningPath[1] == ':' && runningPath[2] == '\0')) {   /* <-- If the running path is empty, it means we're trying to create the root directory. */
                if (!ctk_directory_exists(runningPath)) {
                    ctk_result result = ctk_mkdir(runningPath);
                    if (result != CTK_SUCCESS) {
                        return result;
                    }
                }
            }

            runningPath[i++] = '/';
            runningPath[i]   = '\0';

            if (directoryPath[0] == '\0') {
                break;
            }
        } else {
            runningPath[i++] = directoryPath[0];
        }

        directoryPath += 1;
    }

    return CTK_SUCCESS;
}

ctk_bool32 ctk_iterate_files(const char* directory, ctk_bool32 recursive, ctk_iterate_files_proc proc, void* pUserData)
{
#ifdef _WIN32
    unsigned int searchQueryLength;
    WIN32_FIND_DATAA ffd;
    HANDLE hFind;

    char searchQuery[MAX_PATH];
    ctk_strcpy_s(searchQuery, sizeof(searchQuery), directory);

    searchQueryLength = (unsigned int)strlen(searchQuery);
    if (searchQueryLength >= MAX_PATH - 3) {
        return CTK_FALSE;    /* Path is too long. */
    }

    searchQuery[searchQueryLength + 0] = '\\';
    searchQuery[searchQueryLength + 1] = '*';
    searchQuery[searchQueryLength + 2] = '\0';

    hFind = FindFirstFileA(searchQuery, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return CTK_FALSE; /* Failed to begin search. */
    }

    do
    {
        char filePath[MAX_PATH];

        /* Skip past "." and ".." directories. */
        if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0) {
            continue;
        }

        ctk_strcpy_s(filePath, sizeof(filePath), directory);
        ctk_strcat_s(filePath, sizeof(filePath), "/");
        ctk_strcat_s(filePath, sizeof(filePath), ffd.cFileName);

        if (!proc(filePath, pUserData)) {
            return CTK_FALSE;
        }

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (recursive) {
                if (!ctk_iterate_files(filePath, recursive, proc, pUserData)) {
                    return CTK_FALSE;
                }
            }
        }

    } while (FindNextFileA(hFind, &ffd));

    FindClose(hFind);
#else
    struct dirent* info = NULL;

    DIR* dir = opendir(directory);
    if (dir == NULL) {
        return CTK_FALSE;
    }

    while ((info = readdir(dir)) != NULL)
    {
        char filePath[4096];
        struct stat fileinfo;

        /* Skip past "." and ".." directories. */
        if (strcmp(info->d_name, ".") == 0 || strcmp(info->d_name, "..") == 0) {
            continue;
        }

        ctk_strcpy_s(filePath, sizeof(filePath), directory);
        ctk_strcat_s(filePath, sizeof(filePath), "/");
        ctk_strcat_s(filePath, sizeof(filePath), info->d_name);

        if (stat(filePath, &fileinfo) != 0) {
            continue;
        }

        if (!proc(filePath, pUserData)) {
            return CTK_FALSE;
        }

        if (fileinfo.st_mode & S_IFDIR) {
            if (recursive) {
                if (!ctk_iterate_files(filePath, recursive, proc, pUserData)) {
                    return CTK_FALSE;
                }
            }
        }
    }

    closedir(dir);
#endif

    return CTK_TRUE;
}


char* ctk_get_current_directory()
{
#ifdef CTK_WIN32
    return ctk_get_current_directory__win32();
#endif
#ifdef CTK_POSIX
    return ctk_get_current_directory__posix();
#endif
}

ctk_result ctk_set_current_directory(const char* path)
{
#ifdef CTK_WIN32
    return ctk_set_current_directory__win32(path);
#endif
#ifdef CTK_POSIX
    return ctk_set_current_directory__posix(path);
#endif
}

