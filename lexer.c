#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include <stdlib.h>

char token[MAX_TOKEN_LENGTH];
static int token_index;

static int is_opened;

/* Function to read and validate input arguments */
Status read_and_validate_args(char *argv[], FileDetails *fileDetails)
{
    if(argv[1] == NULL)
    {
        puts("ERROR: Insufficient arguments");
        puts("Usage: ./a.out <.c file>");
        return failure;
    }

    char *str;
    if((str = strstr(argv[1], ".")) == NULL || strcmp(str, ".c"))
    {
        puts("ERROR: Unsupportd file format");
        puts("Usage: ./a.out <.c file>");
        return failure;
    }

    fileDetails->src_fname = argv[1];
    return success;
}

/* Function to open required files */
Status open_files(FileDetails *fileDetails)
{
    fileDetails->fptr_src = fopen(fileDetails->src_fname, "r");

    if(fileDetails->fptr_src == NULL)
    {
        perror("fopen");
        printf("ERROR: Unable to open %s\n", fileDetails->src_fname);
        return failure;
    }
    else
    {
        return success;
    }
}

/* Function to set a token */
Status set_token()
{
    token[token_index] = '\0';
    token_index = 0;
    return success;
}

/* function to generate tokens */
Status lexer(FILE *fptr_src)
{
    char ch;

    while ((ch = getc(fptr_src)) != EOF)
    {
        switch (ch)
        {
            /* handle preprocessor directive */
        case '#':
            handle_preprocessor_directive(fptr_src);
            printf("%-10s -> Preprocssor directive\n", token);
            break;

        case ' ':
        case '\n':
        case '\t':
            *token = '\0';
            break;
        
        /* Handle identifier & keywords */
        case 'a' ... 'z':
        case 'A' ... 'Z':
            handle_keyword(fptr_src);
            int res = is_keyword(token);
            if(res == res_kword)
            {
                printf("%-10s -> Reserved keyword (Data)\n", token);
            }
            else if(res == non_res_kword)
            {
                printf("%-10s -> Reserved keyword (Non Data)\n", token);
            }
            else
            {
                printf("%-10s -> Identifier\n", token);
            }
            break;
        
        // handle comments 
        case '/':
            if((ch = getc(fptr_src)) == '/')
            {
                // Handle single line comment 
                handle_single_line_comment(fptr_src);
            }
            else if(ch == '*')
            {
                // Handle multiline comment 
                handle_multiline_comment(fptr_src);
            }
            else
            {
                fseek(fptr_src, -1, SEEK_CUR);
                printf("%-10s -> Operator\n", "/");
            }
            break;

        /* Handle character constants */
        case '\'':
                handle_character_constant(fptr_src);
                printf("%-10s -> Character Constant\n", token);
            break;

        /* Handle strings */
        case '\"':
                handle_string_literal(fptr_src);
                printf("%-10s -> String Literal\n", token);
                break;

        /* Handle constants */
        case '0' ... '9':
                handle_integral_literal(fptr_src);
                if(strchr(token , '.') != NULL)
                {
                    printf("%-10s -> Float Literal\n", token);
                }
                else
                {
                    printf("%-10s -> Integral Literal\n", token);
                }
                break;

        /* handle special characters */
        default:
                handle_special_characters(fptr_src);
            break;
        }
    }

    /* Error hanlding for missing brackets */
    if(is_opened > 0)
    {
        printf("\nERROR: Missing %d close braces\n", is_opened);
        return success;
    }
    else if(is_opened < 0)
    {
        printf("\nERROR: Missing %d open braces\n", -is_opened);
        return success;
    }
    else
    {
        printf("\nTokens generated successfully\n");
        return success;
    }
}

/* Function to handle preprocessor directives */
Status handle_preprocessor_directive(FILE *fptr_src)
{
    fseek(fptr_src, -1, SEEK_CUR);
    char ch;

    while((ch = getc(fptr_src)) != EOF)
    {
        switch (ch)
        {
        default:
            token[token_index++] = ch;
            break;

        case '\n':
        set_token();
            return success;
        }
    }
}

/* Function to handle keywords */
Status handle_keyword(FILE *fptr_src)
{
    fseek(fptr_src, -1, SEEK_CUR);
    char ch;

    while((ch = getc(fptr_src)) != EOF)
    {
        switch (ch)
        {
        case 'a' ... 'z':
        case 'A' ... 'Z':
        case '0' ... '1':
        case '_':
            token[token_index++] = ch;
            break;
        
        default:
            fseek(fptr_src, -1, SEEK_CUR);
            set_token();
            return success;
            break;
        }
    }
    
}

/* Function to handle single line comments */
Status handle_single_line_comment(FILE *fptr_src)
{
    char ch;
    while ((ch = getc(fptr_src)) != EOF && ch != '\n');
}

/* function to handle multiline comments */
Status handle_multiline_comment(FILE *fptr_src)
{
    char ch;

    while((ch = getc(fptr_src)) != EOF)
    {
        switch (ch)
        {
        case '*':
            if((ch = getc(fptr_src)) == '/')
            {
                return success;
            }
            else if(ch == EOF)
            {
                printf("ERROR: No end of multiline comment\n");
                exit(0);
            }
            break;
        }
    }

    printf("\nERROR: No end of multiline comment\n");
    exit(0);
}

/* Function to handle character constants */
Status handle_character_constant(FILE *fptr_src)
{
    char ch;

    fseek(fptr_src, -1, SEEK_CUR);
    ch = getc(fptr_src);
    token[token_index++] = ch;

    while ((ch = getc(fptr_src)) != EOF)
    {
        switch (ch)
        {
        case '\'':
            token[token_index++] = ch;
            set_token();
            return success;
            break;

        case '\\':
            token[token_index++] = ch;
            token[token_index++] = getc(fptr_src);
            break;

        case '\n':
            set_token();
            printf("ERROR: %s\n", token);
            printf("ERROR: Missing single quote\n");
            exit(0);
            break;

        default:
            token[token_index++] = ch;
            break;
        }
    }
    
    set_token();
    printf("\nERROR: %s\n", token);
    printf("ERROR: Missing single quote\n");
    exit(0);
}

/* Function to handle integral literals */
Status handle_integral_literal(FILE *fptr_src)
{
    char ch;

    fseek(fptr_src, -1, SEEK_CUR);

    while ((ch = getc(fptr_src)) != EOF)
    {
        switch (ch)
        {
        case '0' ... '9':
        case 'L':
        case 'f':
            token[token_index++] = ch;
            break;

        case '.':
            token[token_index++] = ch;
            if((ch = getc(fptr_src)) < '0' || ch > '9')
            {
                set_token();
                printf("ERROR: %s\n", token);
                printf("ERROR: Invalid float literal");
                exit(0);
            }
            else
            {
                token[token_index++] = ch;
            }
            break;

        case 'a' ... 'e':
        case 'g' ... 'z':
        case 'A' ... 'K':
        case 'M' ... 'Z':
            token[token_index++] = ch;
            set_token();
            printf("ERROR: %s\n", token);
            printf("ERROR: Invalid integral literal");
            exit(0);
            break;
        
        default:
            fseek(fptr_src, -1, SEEK_CUR);
            set_token();
            return success;
            break;
        }
    }
    
}

/* Function to handle string literals */
Status handle_string_literal(FILE *fptr_src)
{
    char ch;

    fseek(fptr_src, -1, SEEK_CUR);
    ch = getc(fptr_src);
    token[token_index++] = ch;  // Store '\"'

    while ((ch = getc(fptr_src)) != EOF)
    {
        switch (ch)
        {
        case '\"':
            token[token_index++] = ch;
            set_token();
            return success;
            break;
        
        case '\\':
            token[token_index++] = ch;
            token[token_index++] = getc(fptr_src);
            break;

        case '\n':
            set_token();
            printf("\nERROR: %s\n", token);
            printf("ERROR: Missing double quote\n");
            exit(0);
            break;
            
        default:
            token[token_index++] = ch;
            break;
        }
    }
    set_token();
    printf("\nERROR: %s\n", token);
    printf("ERROR: Missing double quote\n");
    exit(0);

}


/* Function to handle spacial characters */
Status handle_special_characters(FILE *fptr_src)
{
    fseek(fptr_src, -1, SEEK_CUR);

    char ch = getc(fptr_src);
//putchar(ch);
    if(is_operator(ch) == success)
    {
        printf("%-10c -> Operator\n", ch);
    }
    else if(is_open_brace(ch) == success)
    {
        printf("%-10c -> Open brace\n", ch);
        is_opened++;
    }
    else if(is_close_brace(ch) == success)
    {
        printf("%-10c -> close brace\n", ch);
        is_opened--;
    }
    else
    {
        printf("%-10c -> Special character\n", ch);
    }

    return success;
}

/* Function to check if it is an open brace */
Status is_open_brace(char c)
{
	int idx;
    static char open_brace[] = {'{', '[', '('};

	for(idx = 0; idx < 3; idx++)
	{
		if(open_brace[idx] == c)
			return success;
	}

	return failure;
}

/* Function to check if it is a closing brace */
Status is_close_brace(char c)
{
	int idx;

    static char close_brace[] = {'}', ']', ')'};
	for(idx = 0; idx < 3; idx++)
	{
		if(close_brace[idx] == c)
			return success;
	}

	return failure;
}

/* Function to check if it is a operator */
Status is_operator(char c)
{
	int idx;

    static char operators[] = {'/', '+', '*', '-', '%', '=', '<', '>', '~', '&', ',', '!', '^', '|'};

	for(idx = 0; idx < 14; idx++)
	{
		if(operators[idx] == c)
			return success;
	}

	return failure;
}

/* function to check if the token is a keyword */
Status is_keyword(char *token)
{
    static char *res_kwords_data[] = {"const", "volatile", "extern", "auto", "register", "static", 
                            "signed", "unsigned", "short", "long", "double", "char", 
                            "int", "float", "struct", "union", "enum", "void", "typedef", ""};

    static char *res_kwords_non_data[] = {"goto", "return", "continue", "break", "if", "else", "for", 
                                    "while", "do", "switch", "case", "default", "sizeof", ""};

    for(int iter = 0; iter < 20; iter++)
    {
        if(!strcmp(token, res_kwords_data[iter]))
        {
            return res_kword;
        }
    }

    for(int iter = 0; iter < 14; iter++)
    {
        if(!strcmp(token, res_kwords_non_data[iter]))
        {
            return non_res_kword;
        }
    }

    return failure;
}

