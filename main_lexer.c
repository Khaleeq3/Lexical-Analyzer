#include <stdio.h>
#include "type.h"
#include "lexer.h"

int main(int argc, char *argv[])
{
    /* variable to hold file details */
    FileDetails fileDetails;

    /* read_and_validate_args() function call */
    if(read_and_validate_args(argv, &fileDetails) == failure)
    {
        puts("ERROR: read_and_validate_args function failed");
        return 1;
    }

    /* open_files() function call */
    if(open_files(&fileDetails) == failure)
    {
        puts("ERROR: open_files function failed");
        return 1;
    }

    /* lexer() function call */
    if(lexer(fileDetails.fptr_src) != success)
    {
        puts("ERROR: lexer function failed");
        return 1;
    }
    
    return 0;
}