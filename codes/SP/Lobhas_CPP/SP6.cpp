#include <stdio.h> 
#include <ctype.h> 
#include <string.h> 
#include <stdlib.h> 

#define MAX_LEXEMES 100 
#define MAX_LINE_LENGTH 256 
#define MAX_SYMBOLS 100 

struct Lexeme { 
    int line_no; 
    char lexeme[100]; 
    char token[50]; 
    char token_value[50];  
}; 

struct Symbol { 
    char symbol[100]; 
    int index; 
};

int line_no; 
char lexeme[100]; 
char token[50]; 
char token_value[50];  

struct Lexeme lexeme_table[MAX_LEXEMES]; 
int lexeme_count = 0; 

struct Symbol symbol_table[MAX_SYMBOLS]; 
int symbol_count = 0; 

const char *keywords[] = {"int", "float", "char", "if", "else", "while", "for", "return", "void"}; 
const int num_keywords = 9; 

const char *operators[] = {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">="}; 
const int num_operators = 11; 

const char *delimiters[] = {"(", ")", "{", "}", ";", ","}; 
const int num_delimiters = 6; 

int is_keyword(const char *str) { 
    for (int i = 0; i < num_keywords; i++) { 
        if (strcmp(str, keywords[i]) == 0) 
            return i;  
    } 
    return -1; 
} 

int is_operator(const char *str) { 
    for (int i = 0; i < num_operators; i++) { 
        if (strcmp(str, operators[i]) == 0) 
            return i;  
    } 
    return -1; 
} 

int is_delimiter(const char *str) { 
    for (int i = 0; i < num_delimiters; i++) { 
        if (strcmp(str, delimiters[i]) == 0) 
            return i;  
    } 
    return -1; 
} 

int add_symbol(const char *symbol) { 
    for (int i = 0; i < symbol_count; i++) { 
        if (strcmp(symbol_table[i].symbol, symbol) == 0) { 
            return i;  
        } 
    } 
    strcpy(symbol_table[symbol_count].symbol, symbol); 
    symbol_table[symbol_count].index = symbol_count; 
    symbol_count++; 
    return symbol_count - 1;  
} 

void add_lexeme(int line_no, const char *lex, const char *token, const char *value) { 
    lexeme_table[lexeme_count].line_no = line_no; 
    strcpy(lexeme_table[lexeme_count].lexeme, lex); 
    strcpy(lexeme_table[lexeme_count].token, token); 
    strcpy(lexeme_table[lexeme_count].token_value, value); 
    lexeme_count++; 
} 

void lexical_analyzer(FILE *file) { 
    char line[MAX_LINE_LENGTH]; 
    int line_no = 0; 
    int paren_count = 0; 
    while (fgets(line, sizeof(line), file)) { 
        line_no++; 
        char *token = strtok(line, " \t\n"); 
        while (token != NULL) { 
            int index; 
            if ((index = is_keyword(token)) != -1) { 
                char index_str[10]; 
                sprintf(index_str, "%d", index);  
                add_lexeme(line_no, token, "KEYWORD", index_str); 
            } 
            else if ((index = is_operator(token)) != -1) { 
                char index_str[10]; 
                sprintf(index_str, "%d", index);  
                add_lexeme(line_no, token, "OPERATOR", index_str); 
            } 
            else if ((index = is_delimiter(token)) != -1) { 
                char index_str[10]; 
                sprintf(index_str, "%d", index); 
                add_lexeme(line_no, token, "DELIMITER", index_str); 
                if (strcmp(token, "(") == 0) 
                    paren_count++; 
                if (strcmp(token, ")") == 0) 
                    paren_count--; 
            } 
            else if (isdigit(token[0])) { 
                add_lexeme(line_no, token, "Constant", token); 
            } 
            else { 
                int symbol_index = add_symbol(token);  
                char index_str[10]; 
                sprintf(index_str, "%d", symbol_index);  
                add_lexeme(line_no, token, "IDENTIFIER", index_str); 
            } 
            token = strtok(NULL, " \t\n"); 
        } 
    } 
    if (paren_count != 0) { 
        printf("Error: Unmatched parentheses detected!\n"); 
    } 
} 

void write_lexeme_table_to_file() { 
    FILE *output_file = fopen("assign6output.txt", "w"); 
    if (output_file == NULL) { 
        printf("Error: Unable to open output file.\n"); 
        return; 
    } 
    fprintf(output_file, "+--------+---------------+---------------+---------------+\n"); 
    fprintf(output_file, "| %-6s | %-13s | %-13s | %-13s |\n", "Line No", "Lexeme", "Token", "Token Value"); 
    fprintf(output_file, "+--------+---------------+---------------+---------------+\n"); 
    for (int i = 0; i < lexeme_count; i++) { 
        fprintf(output_file, "| %-6d | %-13s | %-13s | %-13s |\n", 
                lexeme_table[i].line_no, 
                lexeme_table[i].lexeme, 
                lexeme_table[i].token, 
                lexeme_table[i].token_value); 
    } 
    fprintf(output_file, "+--------+---------------+---------------+---------------+\n\n"); 
    fprintf(output_file, "+--------+---------------+\n"); 
    fprintf(output_file, "| %-6s | %-13s |\n", "Index", "Symbol"); 
    fprintf(output_file, "+--------+---------------+\n"); 
    for (int i = 0; i < symbol_count; i++) { 
        fprintf(output_file, "| %-6d | %-13s |\n", symbol_table[i].index, symbol_table[i].symbol); 
    } 
    fprintf(output_file, "+--------+---------------+\n"); 

    fclose(output_file); 
} 

int main() { 
    FILE *file = fopen("input6.txt", "r"); 
    if (file == NULL) { 
        printf("Error: Unable to open file.\n"); 
        return 1; 
    } 
    lexical_analyzer(file); 
    write_lexeme_table_to_file(); 
    fclose(file); 
    printf("Lexical analysis completed. Output written to assign6output.txt\n"); 
    return 0; 
} 







