#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LABEL_LEN 10
#define MAX_OPCODE_LEN 10
#define MAX_OPERAND_LEN 10
#define MAX_LITERAL_LEN 10
#define MAX_POOL_LEN 10
#define MAX_SYMBOLS 50
#define MAX_LITERALS 50
#define MAX_OPCODES 50

typedef struct {
    char literal[MAX_LITERAL_LEN];
    int address;
} Literal;

typedef struct {
    char label[MAX_LABEL_LEN];
    int address;
} Symbol;

typedef struct {
    char opcode[MAX_OPCODE_LEN];
    char mnemonic[3];
} Opcode;

Literal littab[MAX_LITERALS];
Symbol symtab[MAX_SYMBOLS];
Opcode optab[MAX_OPCODES];
int littabIndex = 0, symtabIndex = 0, optabIndex = 0, pooltabIndex = 0;
int locctr = 0;
int pooltab[MAX_POOL_LEN];

void processLine(char label[], char opcode[], char operand[]);
int searchOpcode(char opcode[]);
void handleLTORG();
void addLiteral(char literal[]);
void addSymbol(char label[]);
void displayTables();
void loadOptab();
void generateIntermediateCode(char opcode[], char operand[]);

int main() {
    FILE *inputFile = fopen("input2.txt", "r");
    char label[MAX_LABEL_LEN], opcode[MAX_OPCODE_LEN], operand[MAX_OPERAND_LEN];

    if (!inputFile) {
        printf("Error opening input file.\n");
        return 1;
    }

    loadOptab();  // Load the opcode table from file

    while (fscanf(inputFile, "%s %s %s", label, opcode, operand) != EOF) {
        processLine(label, opcode, operand);
    }

    fclose(inputFile);
    displayTables();

    return 0;
}

void processLine(char label[], char opcode[], char operand[]) {
    // If label exists, add to symbol table
    if (strcmp(label, "**") != 0) {
        addSymbol(label);
    }

    // START directive handling
    if (strcmp(opcode, "START") == 0) {
        locctr = atoi(operand);
        printf("IC: (AD,01) C %d\n", locctr);
    }
    // END directive handling
    else if (strcmp(opcode, "END") == 0) {
        handleLTORG(); // Process remaining literals
        printf("IC: (AD,02)\n");
    }
    // LTORG directive handling
    else if (strcmp(opcode, "LTORG") == 0) {
        handleLTORG(); // Process literals at this point
        printf("IC: (AD,03)\n");
    }
    // Check if the opcode is valid
    else {
        int opcodeIndex = searchOpcode(opcode);
        if (opcodeIndex != -1) {
            generateIntermediateCode(opcode, operand);
            if (operand[0] == '=') {
                addLiteral(operand);
            }
            locctr += 3;
        }
        // Data definition statements
        else if (strcmp(opcode, "WORD") == 0) {
            printf("IC: (DL,01) C %s\n", operand);
            locctr += 3;
        } else if (strcmp(opcode, "RESW") == 0) {
            printf("IC: (DL,02) C %s\n", operand);
            locctr += 3 * atoi(operand);
        } else if (strcmp(opcode, "BYTE") == 0) {
            printf("IC: (DL,03) C %s\n", operand);
            locctr += strlen(operand) - 3;
        } else if (strcmp(opcode, "RESB") == 0) {
            printf("IC: (DL,04) C %s\n", operand);
            locctr += atoi(operand);
        } 
        // Handle invalid opcodes
        else {
            printf("Error: Invalid opcode '%s'\n", opcode);
        }
    }
}

int searchOpcode(char opcode[]) {
    for (int i = 0; i < optabIndex; i++) {
        if (strcmp(opcode, optab[i].opcode) == 0) {
            return i;
        }
    }
    return -1;
}

void handleLTORG() {
    for (int i = pooltab[pooltabIndex]; i < littabIndex; i++) {
        littab[i].address = locctr;
        locctr += 3;
    }
    pooltab[++pooltabIndex] = littabIndex;
}

void addLiteral(char literal[]) {
    strcpy(littab[littabIndex].literal, literal);
    littab[littabIndex].address = -1;  // Will be updated in LTORG
    littabIndex++;
}

void addSymbol(char label[]) {
    strcpy(symtab[symtabIndex].label, label);
    symtab[symtabIndex].address = locctr;
    symtabIndex++;
}

void displayTables() {
    printf("Literal Table:\n");
    for (int i = 0; i < littabIndex; i++) {
        printf("%s\t%d\n", littab[i].literal, littab[i].address);
    }

    printf("\nPool Table:\n");
    for (int i = 0; i <= pooltabIndex; i++) {
        printf("%d\n", pooltab[i]);
    }

    printf("\nSymbol Table:\n");
    for (int i = 0; i < symtabIndex; i++) {
        printf("%s\t%d\n", symtab[i].label, symtab[i].address);
    }
}

void loadOptab() {
    FILE *optabFile = fopen("optab2.txt", "r");
    if (!optabFile) {
        printf("Error opening optab file.\n");
        exit(1);
    }

    while (fscanf(optabFile, "%s %s", optab[optabIndex].opcode, optab[optabIndex].mnemonic) != EOF) {
        optabIndex++;
    }
    fclose(optabFile);
}
void generateIntermediateCode(char opcode[], char operand[]) {
    int opcodeIndex = searchOpcode(opcode);
    if (operand[0] == '=') {
        printf("IC: (%s,%s) %s\n", optab[opcodeIndex].mnemonic, optab[opcodeIndex].opcode, operand);
    } else {
        printf("IC: (%s,%s) S\n", optab[opcodeIndex].mnemonic, optab[opcodeIndex].opcode);
    }
}















