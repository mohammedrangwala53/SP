#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100

// Structures for OPTAB and SYMTAB
typedef struct {
    char mnemonic[10];
    char opcode[5];
} OPTAB;

typedef struct {
    char label[10];
    int locctr;
} SYMTAB;

// Function to search in OPTAB
int searchOptab(OPTAB optab[], int size, char *mnemonic) {
    for (int i = 0; i < size; i++) {
        if (strcmp(optab[i].mnemonic, mnemonic) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to search in SYMTAB
int searchSymtab(SYMTAB symtab[], int size, char *label) {
    for (int i = 0; i < size; i++) {
        if (strcmp(symtab[i].label, label) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    FILE *intermediateFile, *optabFile, *symtabFile, *outputFile;
    OPTAB optab[MAX_LEN];
    SYMTAB symtab[MAX_LEN];
    char line[100], label[10], opcode[10], operand[10];
    int optabSize = 0, symtabSize = 0;
    int locctr;

    // Open the input files
    intermediateFile = fopen("intermediate.txt", "r");
    optabFile = fopen("optab.txt", "r");
    symtabFile = fopen("symtab.txt", "r");
    outputFile = fopen("machine_code.txt", "w");

    if (!intermediateFile || !optabFile || !symtabFile || !outputFile) {
        printf("Error opening files.\n");
        return 1;
    }

    // Read OPTAB
    while (fscanf(optabFile, "%s %s", optab[optabSize].mnemonic, optab[optabSize].opcode) != EOF) {
        optabSize++;
    }
    fclose(optabFile);

    // Read SYMTAB
    while (fscanf(symtabFile, "%s %d", symtab[symtabSize].label, &symtab[symtabSize].locctr) != EOF) {
        symtabSize++;
    }
    fclose(symtabFile);

    // Process Intermediate File
    fprintf(outputFile, "LocCtr    Machine Code\n");
    while (fgets(line, sizeof(line), intermediateFile)) {
        // Reset variables
        label[0] = opcode[0] = operand[0] = '\0';

        // Read the line and extract tokens
        int readCount = sscanf(line, "%d %s %s %s", &locctr, label, opcode, operand);

        if (readCount < 2) continue; // Skip empty or invalid lines

        // Handle START directive
        if (strcmp(opcode, "START") == 0) {
            fprintf(outputFile, "%04d    START  %s\n", locctr, operand);
            continue;
        }

        // Handle assembler directives
        if (strcmp(opcode, "RESW") == 0) {
            // Reserve words (allocate memory, no machine code)
            fprintf(outputFile, "%04d    \n", locctr);
            continue;
        }
        if (strcmp(opcode, "RESB") == 0) {
            // Reserve bytes (allocate memory, no machine code)
            fprintf(outputFile, "%04d    \n", locctr);
            continue;
        }
        if (strcmp(opcode, "WORD") == 0) {
            // Define a word-sized constant
            fprintf(outputFile, "%04d    %06X\n", locctr, atoi(operand));
            continue;
        }
        if (strcmp(opcode, "BYTE") == 0) {
            // Define a byte-sized constant (e.g., C'Z' or X'1A')
            if (operand[0] == 'C') {
                fprintf(outputFile, "%04d    %02X\n", locctr, operand[2]);
            } else if (operand[0] == 'X') {
                int value;
                sscanf(&operand[2], "%2x", &value);
                fprintf(outputFile, "%04d    %02X\n", locctr, value);
            }
            continue;
        }

        // Handle END directive
        if (strcmp(opcode, "END") == 0) {
            fprintf(outputFile, "%04d    END\n", locctr);
            break;
        }

        // Search for opcode in OPTAB
        int optabIndex = searchOptab(optab, optabSize, opcode);
        if (optabIndex == -1) {
            printf("Error: Invalid mnemonic %s on line %d\n", opcode, locctr);
            continue;
        }

        // Generate machine code
        char machineCode[10] = "";
        strcpy(machineCode, optab[optabIndex].opcode);

        // Search operand in SYMTAB
        int symtabIndex = searchSymtab(symtab, symtabSize, operand);
        if (symtabIndex != -1) {
            sprintf(machineCode + strlen(machineCode), "%04d", symtab[symtabIndex].locctr);
        } else {
            strcat(machineCode, "0000"); // Default for no operand or invalid symbols
        }

        // Write machine code
        fprintf(outputFile, "%04d    %s\n", locctr, machineCode);
    }

    fclose(intermediateFile);
    fclose(outputFile);

    printf("Pass-II completed. Output written to machine_code.txt\n");
    return 0;
}
