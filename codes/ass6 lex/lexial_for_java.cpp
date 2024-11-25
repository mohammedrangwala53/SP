 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token tables for Java
typedef struct {
    const char *keyword;
    int token;
} KeywordToken;

KeywordToken javaKeywords[] = {
    {"abstract", 1}, {"assert", 2}, {"boolean", 3}, {"break", 4}, {"byte", 5},
    {"case", 6}, {"catch", 7}, {"char", 8}, {"class", 9}, {"const", 10},
    {"continue", 11}, {"default", 12}, {"do", 13}, {"double", 14}, {"else", 15},
    {"enum", 16}, {"extends", 17}, {"final", 18}, {"finally", 19}, {"float", 20},
    {"for", 21}, {"goto", 22}, {"if", 23}, {"implements", 24}, {"import", 25},
    {"instanceof", 26}, {"int", 27}, {"interface", 28}, {"long", 29}, {"native", 30},
    {"new", 31}, {"null", 32}, {"package", 33}, {"private", 34}, {"protected", 35},
    {"public", 36}, {"return", 37}, {"short", 38}, {"static", 39}, {"strictfp", 40},
    {"super", 41}, {"switch", 42}, {"synchronized", 43}, {"this", 44}, {"throw", 45},
    {"throws", 46}, {"transient", 47}, {"try", 48}, {"void", 49}, {"volatile", 50},
    {"while", 51}
};
const int keywordCount = sizeof(javaKeywords) / sizeof(javaKeywords[0]);

const char *javaOperators[] = {
    "+", "-", "*", "/", "%", "=", "==", "!=", ">", "<", ">=", "<=", "&&", "||",
    "!", "&", "|", "^", "<<", ">>", ">>>"
};
const int operatorCount = sizeof(javaOperators) / sizeof(javaOperators[0]);

const char javaSymbols[] = {',', ';', '(', ')', '{', '}', '[', ']', '.'};
const int symbolCount = sizeof(javaSymbols) / sizeof(javaSymbols[0]);

int isKeyword(const char *str) {
    for (int i = 0; i < keywordCount; i++) {
        if (strcmp(str, javaKeywords[i].keyword) == 0) {
            return javaKeywords[i].token;
        }
    }
    return -1;
}

int isOperator(const char *str) {
    for (int i = 0; i < operatorCount; i++) {
        if (strcmp(str, javaOperators[i]) == 0) {
            return 1; // Return 1 for any valid operator
        }
    }
    return 0;
}

int isSymbol(char ch) {
    for (int i = 0; i < symbolCount; i++) {
        if (ch == javaSymbols[i]) {
            return 1; // Return 1 for any valid symbol
        }
    }
    return 0;
}

int isValidIdentifier(const char *str) {
    if (!isalpha(str[0]) && str[0] != '_') {
        return 0;
    }
    for (int i = 1; str[i] != '\0'; i++) {
        if (!isalnum(str[i]) && str[i] != '_') {
            return 0;
        }
    }
    return 1;
}

void lexicalAnalysis(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    char ch, buffer[100];
    int lineNo = 1, i = 0;

    printf("┌────────────┬───────────────┬──────────────┬───────────────────────┐\n");
    printf("│ %-10s │ %-13s │ %-12s │ %-18s │\n", "Line No", "Lexeme", "Token", "Token Value");
    printf("├────────────┼───────────────┼──────────────┼───────────────────────┤\n");

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lineNo++;
        }

        if (isspace(ch)) {
            continue;
        }

        // Handle keywords and identifiers
        if (isalpha(ch) || ch == '_') {
            i = 0;
            buffer[i++] = ch;
            while ((ch = fgetc(file)) != EOF && (isalnum(ch) || ch == '_')) {
                buffer[i++] = ch;
            }
            buffer[i] = '\0';
            ungetc(ch, file);

            int keywordToken = isKeyword(buffer);
            if (keywordToken != -1) {
                printf("│ %-10d │ %-13s │ %-12s │ %-18d │\n", lineNo, buffer, "Keyword", keywordToken);
            } else if (isValidIdentifier(buffer)) {
                printf("│ %-10d │ %-13s │ %-12s │ %-18d │\n", lineNo, buffer, "Identifier", 100);
            } else {
                printf("│ %-10d │ %-13s │ %-12s │ %-18s │\n", lineNo, buffer, "Error", "Invalid Identifier");
            }
        }

        // Handle numeric constants
        else if (isdigit(ch)) {
            i = 0;
            buffer[i++] = ch;
            while ((ch = fgetc(file)) != EOF && (isdigit(ch) || ch == '.')) {
                buffer[i++] = ch;
            }
            buffer[i] = '\0';
            ungetc(ch, file);
            printf("│ %-10d │ %-13s │ %-12s │ %-18d │\n", lineNo, buffer, "Numeric", 101);
        }

        // Handle operators
        else if (strchr("+-*/%!=<>|&", ch)) {
            i = 0;
            buffer[i++] = ch;
            if (strchr("=|&<>", ch)) {
                char next = fgetc(file);
                if (next == '=' || (ch == next && strchr("|&", ch))) {
                    buffer[i++] = next;
                } else {
                    ungetc(next, file);
                }
            }
            buffer[i] = '\0';
            if (isOperator(buffer)) {
                printf("│ %-10d │ %-13s │ %-12s │ %-18d │\n", lineNo, buffer, "Operator", 102);
            }
        }

        // Handle symbols
        else if (isSymbol(ch)) {
            printf("│ %-10d │ %-13c │ %-12s │ %-18d │\n", lineNo, ch, "Symbol", 103);
        }

        // Handle unknown characters
        else {
            printf("│ %-10d │ %-13c │ %-12s │ %-18s │\n", lineNo, ch, "Error", "Unknown");
        }
    }

    printf("└────────────┴───────────────┴──────────────┴───────────────────────┘\n");
    fclose(file);
}

int main() {
    lexicalAnalysis("lexicalinput_java.txt");
    return 0;
}
