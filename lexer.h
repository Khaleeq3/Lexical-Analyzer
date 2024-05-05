#ifndef LEXER_H
#define LEXER_H
#include "type.h"

#define MAX_TOKEN_LENGTH 1024

/* Structure to hold file details */
typedef struct _FileDetails{
    char *src_fname;
    FILE *fptr_src;
} FileDetails;

/* Function to read and validate input arguments */
Status read_and_validate_args(char *argv[], FileDetails *fileDetails);

/* Function to open required files */
Status open_files(FileDetails *fileDetails);

/* Function to set a token */
Status set_token();

/* function to generate tokens */
Status lexer(FILE *fptr_src);

/* Function to handle preprocessor directives */
Status handle_preprocessor_directive(FILE *fptr_src);

/* Function to handle keywords */
Status handle_keyword(FILE *fptr_src);

/* Function to handle single line comments */
Status handle_single_line_comment(FILE *fptr_src);

/* function to handle multiline comments */
Status handle_multiline_comment(FILE *fptr_src);

/* Function to handle character constants */
Status handle_character_constant(FILE *fptr_src);

/* Function to handle integral literals */
Status handle_integral_literal(FILE *fptr_src);

/* Function to handle string literals */
Status handle_string_literal(FILE *fptr_src);

/* Function to handle spacial characters */
Status handle_special_characters(FILE *fptr_src);

/* Function to check if it is an open brace */
Status is_open_brace(char c);

/* Function to check if it is a closing brace */
Status is_close_brace(char c);

/* Function to check if it is a operator */
Status is_operator(char c);

/* function to check if the token is a keyword */
Status is_keyword(char *token);

#endif