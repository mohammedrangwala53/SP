#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int",
    "long", "register", "return", "short", "signed", "sizeof",
    "static",
    "struct", "switch", "typedef", "union", "unsigned", "void",
    "volatile", "while"};

const char *operators[] = {
    "==", "&&", "||", "++", "--", "<=", "!=", "=", ">", "<", "!", "%",
    "*", "+", "-", "/",
    "++", "--", "&&", "||" // Removed duplicate entries
};

const char *delimiters[] = {
    ";", ",", "(", ")", "{", "}", "[", "]"};

char *symbol_table[1000];
int symbol_table_size = 0;

char *constant_table[1000];
int constant_table_size = 0;

int get_keyword_index(char *lexeme)
{
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
    {
        if (strcmp(lexeme, keywords[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

int get_identifier_index(char *lexeme)
{
    for (int i = 0; i < symbol_table_size; i++)
    {
        if (strcmp(lexeme, symbol_table[i]) == 0)
        {
            return i;
        }
    }
    symbol_table[symbol_table_size++] = strdup(lexeme);
    return symbol_table_size - 1;
}

int get_constant_index(char *lexeme)
{
    for (int i = 0; i < constant_table_size; i++)
    {
        if (strcmp(lexeme, constant_table[i]) == 0)
        {
            return i;
        }
    }
    constant_table[constant_table_size++] = strdup(lexeme);
    return constant_table_size - 1;
}

int get_operator_index(char *op)
{
    for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++)
    {
        if (strcmp(op, operators[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

int get_delimiter_index(char *delimiter)
{
    for (int i = 0; i < sizeof(delimiters) / sizeof(delimiters[0]); i++)
    {
        if (strcmp(delimiter, delimiters[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

// Tokenizing function that allows separate analysis of brackets
void analyze_file(const char *file_name)
{
    FILE *fp = fopen(file_name, "r");
    if (fp == NULL)
    {
        printf("Error: Could not open file %s\n", file_name);
        return;
    }

    char line[1024];
    int line_no = 0;
    while (fgets(line, sizeof(line), fp))
    {
        line_no++;
        char *token = strtok(line, " \t\n\r");

        while (token != NULL)
        {
            // Handle individual characters that are delimiters (like brackets)
            for (int i = 0; token[i] != '\0'; i++)
            {
                char ch = token[i];

                // Check for operators and delimiters (including brackets)
                if (strchr("+-*/=><!%&|", ch) != NULL)
                {
                    // If it's an operator
                    if (i < strlen(token) - 1 && strchr("=><!",
                                                        token[i + 1]))
                    {
                        char op[3] = {ch, token[i + 1], '\0'};
                        int operator_index = get_operator_index(op);
                        if (operator_index != -1)
                        {
                            printf("%d | %s | Operator | %d\n", line_no,
                                   op, operator_index);
                            i++; // Skip the next character since it's part of the operator
                        }
                    }
                    else
                    {
                        // Handle single character operators
                        char op[2] = {ch, '\0'};
                        int operator_index = get_operator_index(op);
                        if (operator_index != -1)
                        {
                            printf("%d | %s | Operator | %d\n", line_no,
                                   op, operator_index);
                        }
                    }
                }
                else if (strchr(";(),{}[]", ch) != NULL)
                {
                    // If it's a delimiter
                    char delim[2] = {ch, '\0'};
                    int delimiter_index = get_delimiter_index(delim);
                    if (delimiter_index != -1)
                    {
                        printf("%d | %c | Delimiter | %d\n", line_no, ch,
                               delimiter_index);
                    }
                }
                else if (isalpha(ch) || ch == '_')
                {
                    // Handle identifiers and keywords
                    char identifier[100];
                    int j = 0;
                    while (isalnum(ch) || ch == '_')
                    {
                        identifier[j++] = ch;
                        ch = token[++i];
                    }
                    identifier[j] = '\0';
                    i--; // Move back one position for the outer loop
                    int keyword_index = get_keyword_index(identifier);
                    if (keyword_index != -1)
                    {
                        printf("%d | %s | Keyword | %d\n", line_no,
                               identifier, keyword_index);
                    }
                    else
                    {
                        int identifier_index =
                            get_identifier_index(identifier);
                        printf("%d | %s | Identifier | %d\n", line_no,
                               identifier, identifier_index);
                    }
                }
                else if (isdigit(ch))
                {
                    // Handle constants
                    char constant[100];
                    int j = 0;
                    while (isdigit(ch))
                    {
                        constant[j++] = ch;
                        ch = token[++i];
                    }
                    constant[j] = '\0';
                    i--; // Move back one position for the outer loop
                    int constant_index = get_constant_index(constant);
                    printf("%d | %s | Constant | %d\n", line_no, constant,
                           constant_index);
                }
            }
            token = strtok(NULL, " \t\n\r");
        }
    }

    fclose(fp);

    printf("\nSymbol Table:\n");
    for (int i = 0; i < symbol_table_size; i++)
    {
        printf("%d | %s\n", i, symbol_table[i]);
    }

    printf("\nConstant Table:\n");
    for (int i = 0; i < constant_table_size; i++)
    {
        printf("%d | %s\n", i, constant_table[i]);
    }

    printf("\nOperators:\n");
    for (int i = 0; i < sizeof(operators) / sizeof(operators[0]); i++)
    {
        printf("%d | %s\n", i, operators[i]);
    }

    printf("\nDelimiters:\n");
    for (int i = 0; i < sizeof(delimiters) / sizeof(delimiters[0]); i++)
    {
        printf("%d | %s\n", i, delimiters[i]);
    }

    printf("\nKeywords:\n");
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
    {
        printf("%d | %s\n", i, keywords[i]);
    }
}

int main()
{
    analyze_file("input.c");
    return 0;
}