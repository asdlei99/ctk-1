
ctk_bool32 ctk_pe_reader_is_32bit(const ctk_pe_reader* pReader)
{
    if (pReader == NULL) {
        return CTK_FALSE;
    }

    return pReader->header.magic == 0x010B;
}

ctk_bool32 ctk_pe_reader_is_64bit(const ctk_pe_reader* pReader)
{
    if (pReader == NULL) {
        return CTK_FALSE;
    }

    return pReader->header.magic == 0x020B;
}