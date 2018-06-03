
typedef struct
{
    ctk_uint16 magic;
    ctk_uint8  linkerVersionMajor;
    ctk_uint8  linkerVersionMinor;
    ctk_uint32 textSizeInBytes;
    ctk_uint32 initializedDataSizeInBytes;
    ctk_uint32 uninitializedDataSizeInBytes;
    ctk_uint32 entryPointOffset;
    ctk_uint32 textOffset;
    ctk_uint32 dataOffset;
} ctk_pe32_optional_header;

typedef struct
{
    ctk_uint16 magic;
    ctk_uint8  linkerVersionMajor;
    ctk_uint8  linkerVersionMinor;
    ctk_uint32 textSizeInBytes;
    ctk_uint32 initializedDataSizeInBytes;
    ctk_uint32 uninitializedDataSizeInBytes;
    ctk_uint32 entryPointOffset;
    ctk_uint32 textOffset;
} ctk_pe64_optional_header;