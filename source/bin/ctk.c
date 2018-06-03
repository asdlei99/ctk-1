#define CTK_IMPLEMENTATION
#include "../libctk.h"


void print_version()
{
    printf("ctk version %s\n", CTK_VERSION);
}

void print_help()
{
    printf("Usage: ctk [file] [options]\n");
}

int main(int argc, char** argv)
{
    const char* pInputFile;

    if (argc < 2) {
        printf("No input.\n");
        return -1;
    }

    if (strcmp(argv[1], "-v") == 0) {
        print_version();
        return 0;
    }
    if (strcmp(argv[1], "-h") == 0) {
        print_help();
        return 0;
    }

    /* Currently, the input file needs to be the first argument. */
    pInputFile = argv[1];
    if (ctk_string_is_null_or_empty(pInputFile)) {
        printf("No input files.\n");
        return -1;
    }


    


    CTK_UNUSED(argc);
    CTK_UNUSED(argv);
    return 0;
}
