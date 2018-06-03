
typedef struct
{
    ctk_uint16 magic;
    ctk_uint16 sectionCount;
    ctk_uint32 timeStamp;
    ctk_uint32 symbolTableOffset;
    ctk_uint32 symbolCount;
    ctk_uint16 optionalHeaderSize;
    ctk_uint16 flags;
} ctk_coff_header;

typedef struct
{
    char name[8];
    ctk_uint32 virtualSize;
    ctk_uint32 virtualAddress;
    ctk_uint32 rawDataSize;
    ctk_uint32 rawDataAddress;
    ctk_uint32 relocationsOffset;
    ctk_uint32 lineNumbersOffset;
    ctk_uint16 relocationCount;
    ctk_uint16 lineNumberCount;
    ctk_uint32 flags;
} ctk_coff_section_header;


/*
COFF Reader
*/
typedef struct ctk_coff_reader ctk_coff_reader;
typedef size_t     (* ctk_coff_reader_read_callback)(ctk_coff_reader* pReader, size_t bytesToRead, void* pBytesRead);
typedef ctk_bool32 (* ctk_coff_reader_seek_callback)(ctk_coff_reader* pReader, ctk_int32 offset, ctk_seek_origin origin);

struct ctk_coff_reader
{
    ctk_coff_reader_read_callback onRead;
    ctk_coff_reader_seek_callback onSeek;
    void* pUserData;
    struct
    {
        const ctk_uint8* pData;
        size_t sizeInBytes;
        size_t currentPos;
    } memory;

    ctk_coff_header header;
};

ctk_result ctk_coff_reader_init(ctk_coff_reader_read_callback onRead, ctk_coff_reader_seek_callback onSeek, void* pUserData, ctk_coff_reader* pReader);
ctk_result ctk_coff_reader_init_file(const char* filePath, ctk_coff_reader* pReader);
ctk_result ctk_coff_reader_init_memory(const void* pData, size_t sizeInBytes, ctk_coff_reader* pReader);
void ctk_coff_reader_uninit(ctk_coff_reader* pReader);

ctk_result ctk_coff_reader_read_section_header(ctk_coff_reader* pReader, ctk_uint32 index, ctk_coff_section_header* pHeader);


/*
COFF Writer
*/
typedef struct
{
    int unused;
} ctk_coff_writer;
