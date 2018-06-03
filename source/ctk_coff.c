
/**************************************************************************************************************************************************************
*
* COFF Reader
*
***************************************************************************************************************************************************************/

static ctk_result ctk_coff_reader_preinit(ctk_coff_reader* pReader)
{
    if (pReader == NULL) {
        return CTK_INVALID_ARGS;
    }

    ctk_zero_object(pReader);

    return CTK_SUCCESS;
}

static ctk_result ctk_coff_reader_init__internal(ctk_coff_reader_read_callback onRead, ctk_coff_reader_seek_callback onSeek, void* pUserData, ctk_coff_reader* pReader)
{
    ctk_assert(pReader != NULL);

    pReader->onRead    = onRead;
    pReader->onSeek    = onSeek;
    pReader->pUserData = pUserData;

    /* First thing to do is read the header. */
    if (pReader->onRead(pReader, sizeof(pReader->header), &pReader->header) != sizeof(pReader->header)) {
        return CTK_INVALID_FILE;
    }

    /* When compiling with /GL in Visual Studio (Link Time Optimizations), the format of the COFF file is non-standard. I can't find
       documentation for this so I'm going to fail in this case. */
    if (pReader->header.magic == 0x0000 && pReader->header.sectionCount == 0xFFFF) {
        return CTK_INVALID_FILE;
    }



    return CTK_SUCCESS;
}

ctk_result ctk_coff_reader_init(ctk_coff_reader_read_callback onRead, ctk_coff_reader_seek_callback onSeek, void* pUserData, ctk_coff_reader* pReader)
{
    ctk_result result;
    
    result = ctk_coff_reader_preinit(pReader);
    if (result != CTK_SUCCESS) {
        return result;
    }

    result = ctk_coff_reader_init__internal(onRead, onSeek, pUserData, pReader);
    if (result != CTK_SUCCESS) {
        return result;
    }

    return result;
}


size_t ctk_coff_reader_on_read__memory(ctk_coff_reader* pReader, size_t bytesToRead, void* pData)
{
    size_t bytesRemaining;

    ctk_assert(pReader != NULL);
    ctk_assert(bytesToRead > 0);
    ctk_assert(pData != NULL);
    ctk_assert(pReader->memory.currentPos <= pReader->memory.sizeInBytes);

    bytesRemaining = pReader->memory.sizeInBytes - pReader->memory.currentPos;
    if (bytesRemaining == 0) {
        return 0;
    }

    if (bytesToRead > bytesRemaining) {
        bytesToRead = bytesRemaining;
    }

    ctk_copy_memory(pData, pReader->memory.pData + pReader->memory.currentPos, bytesToRead);
    pReader->memory.currentPos += bytesToRead;

    return bytesToRead;
}

ctk_bool32 ctk_coff_reader_on_seek__memory(ctk_coff_reader* pReader, ctk_int32 offset, ctk_seek_origin origin)
{
    ctk_assert(pReader != NULL);
    ctk_assert(pReader->memory.currentPos <= pReader->memory.sizeInBytes);

    if (origin == ctk_seek_origin_start) {
        if (offset < 0) {
            return CTK_FALSE;
        }

        if ((size_t)offset > pReader->memory.sizeInBytes) {
            return CTK_FALSE;
        }

        pReader->memory.currentPos = (ctk_uint32)offset;    /* Safe cast because we've already checked for negative. */
    } else {
        if (offset > 0) {
            /* Moving forward. */
            size_t maxOffset = pReader->memory.sizeInBytes - pReader->memory.currentPos;
            if ((size_t)offset > maxOffset) {
                return CTK_FALSE;
            }
        } else {
            /* Moving backward */
            size_t maxOffset = pReader->memory.currentPos;
            if ((size_t)(-offset) > maxOffset) {
                return CTK_FALSE;
            }
        }

        pReader->memory.currentPos += offset;
    }

    return CTK_SUCCESS;
}

ctk_result ctk_coff_reader_init_memory(const void* pData, size_t sizeInBytes, ctk_coff_reader* pReader)
{
    ctk_result result;
    
    result = ctk_coff_reader_preinit(pReader);
    if (result != CTK_SUCCESS) {
        return result;
    }

    pReader->memory.pData       = pData;
    pReader->memory.sizeInBytes = sizeInBytes;
    pReader->memory.currentPos  = 0;

    result = ctk_coff_reader_init__internal(ctk_coff_reader_on_read__memory, ctk_coff_reader_on_seek__memory, NULL, pReader);
    if (result != CTK_SUCCESS) {
        return result;
    }

    return result;
}



size_t ctk_coff_reader_on_read__file(ctk_coff_reader* pReader, size_t bytesToRead, void* pData)
{
    ctk_assert(pReader != NULL);
    ctk_assert(bytesToRead > 0);
    ctk_assert(pData != NULL);

    return fread(pData, 1, bytesToRead, (FILE*)pReader->pUserData);
}

ctk_bool32 ctk_coff_reader_on_seek__file(ctk_coff_reader* pReader, ctk_int32 offset, ctk_seek_origin origin)
{
    ctk_assert(pReader != NULL);

    if (fseek((FILE*)pReader->pUserData, offset, (origin == ctk_seek_origin_start) ? SEEK_SET : SEEK_CUR) == 0) {
        return CTK_TRUE;
    } else {
        return CTK_FALSE;
    }
}

ctk_result ctk_coff_reader_init_file(const char* filePath, ctk_coff_reader* pReader)
{
    ctk_result result;
    FILE* pFile;
    
    result = ctk_coff_reader_preinit(pReader);
    if (result != CTK_SUCCESS) {
        return result;
    }

    result = ctk_fopen(filePath, "rb", &pFile);
    if (result != CTK_SUCCESS) {
        return result;
    }

    result = ctk_coff_reader_init__internal(ctk_coff_reader_on_read__file, ctk_coff_reader_on_seek__file, pFile, pReader);
    if (result != CTK_SUCCESS) {
        fclose(pFile);
        return result;
    }

    return result;
}



void ctk_coff_reader_uninit(ctk_coff_reader* pReader)
{
    if (pReader == NULL) {
        return;
    }

    if (pReader->onRead == ctk_coff_reader_on_read__file) {
        fclose((FILE*)pReader->pUserData);
    }
}


ctk_result ctk_coff_reader_read_section_header(ctk_coff_reader* pReader, ctk_uint32 index, ctk_coff_section_header* pHeader)
{
    if (pReader == NULL || index >= pReader->header.sectionCount || pHeader == NULL) {
        return CTK_INVALID_ARGS;
    }

    /* First we need to seek to the start of the section header table. */
    if (!pReader->onSeek(pReader, sizeof(ctk_coff_header) + pReader->header.optionalHeaderSize + (index * sizeof(ctk_coff_section_header)), ctk_seek_origin_start)) {
        return CTK_INVALID_OPERATION;
    }

    if (pReader->onRead(pReader, sizeof(ctk_coff_section_header), pHeader) != sizeof(ctk_coff_section_header)) {
        return CTK_INVALID_ARGS;
    }

    return CTK_SUCCESS;
}




/**************************************************************************************************************************************************************
*
* COFF Writer
*
***************************************************************************************************************************************************************/