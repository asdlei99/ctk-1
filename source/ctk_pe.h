
/* NOTE: This structure is not a 1:1 mapping of the PE/PE+ optional header due to differences in 32- and 64-bit versions. For
         simplicity and code reuse I'm leaving everything as a single structure. */
typedef struct
{
    ctk_coff_header coffHeader;
    ctk_uint16 magic;
    ctk_uint8  linkerVersionMajor;
    ctk_uint8  linkerVersionMinor;
    ctk_uint32 textSizeInBytes;
    ctk_uint32 initializedDataSizeInBytes;
    ctk_uint32 uninitializedDataSizeInBytes;
    ctk_uint32 entryPointOffset;
    ctk_uint32 textOffset;
    ctk_uint32 dataOffset;  /* Unused in PE+. */
} ctk_pe32_optional_header;

typedef struct
{
    ctk_pe32_optional_header header;
} ctk_pe_reader;

ctk_bool32 ctk_pe_reader_is_32bit(const ctk_pe_reader* pReader);
ctk_bool32 ctk_pe_reader_is_64bit(const ctk_pe_reader* pReader);