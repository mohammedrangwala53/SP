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

void processLine(const char label[], const char opcode[], const char operand[]);
int searchOpcode(const char opcode[]);
void handleLTORG();
void addLiteral(const char literal[]);
void addSymbol(const char label[]);
void displayTables();
void loadOptab();
void generateIntermediateCode(const char opcode[], const char operand[]);

int main() {
    char label[MAX_LABEL_LEN], opcode[MAX_OPCODE_LEN], operand[MAX_OPERAND_LEN];

    loadOptab();  // Load the opcode table from embedded data

    // Embedding assembly instructions directly into the source code
    const char *assemblyCode[][3] = {
        {"**", "START", "100"},
        {"**", "LDA", "ALPHA"},
        {"**", "STA", "=5"},
        {"**", "LDCH", "=A"},
        {"**", "LTORG", "**"},
        {"ALPHA", "RESW", "1"},
        {"**", "LDA", "=1"},
        {"**", "STA", "=2"},
        {"**", "LTORG", "**"},
        {"BETA", "RESW", "1"},
        {"**", "END", "**"}
    };

    int n = sizeof(assemblyCode) / sizeof(assemblyCode[0]);

    for (int i = 0; i < n; i++) {
        strcpy(label, assemblyCode[i][0]);
        strcpy(opcode, assemblyCode[i][1]);
        strcpy(operand, assemblyCode[i][2]);

        processLine(label, opcode, operand);
    }

    displayTables();

    return 0;
}

void processLine(const char label[], const char opcode[], const char operand[]) {
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

int searchOpcode(const char opcode[]) {
    for (int i = 0; i < optabIndex; i++) {
        if (strcmp(opcode, optab[i].opcode) == 0) {
            return i;
        }
    }
    return -1;
}

void handleLTORG() {
    for (int i = pooltab[pooltabIndex]; i < littabIndex; i++) {
        littab[i].address = locctr;  // Update literal address in table
        locctr += 3;  // Increment locctr for each literal
    }
    pooltab[++pooltabIndex] = littabIndex;  // Update pooltab to the next literal index
}

void addLiteral(const char literal[]) {
    for (int i = 0; i < littabIndex; i++) {
        if (strcmp(littab[i].literal, literal) == 0) {
            return; // Literal already exists
        }
    }

    memset(littab[littabIndex].literal, 0, MAX_LITERAL_LEN);
    strcpy(littab[littabIndex].literal, literal);
    littab[littabIndex].address = -1;  // Will be updated in LTORG
    littabIndex++;
}

void addSymbol(const char label[]) {
    for (int i = 0; i < symtabIndex; i++) {
        if (strcmp(symtab[i].label, label) == 0) {
            return; // Symbol already exists
        }
    }

    memset(symtab[symtabIndex].label, 0, MAX_LABEL_LEN);
    strcpy(symtab[symtabIndex].label, label);
    symtab[symtabIndex].address = locctr;  // Update address of symbol
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
    // Embedding an expanded opcode table within the code
    const char *opcodes[][2] = {
        {"LDA", "01"},
        {"STA", "02"},
        {"LDCH", "03"},
        {"STCH", "04"},
        {"ADD", "05"},
        {"SUB", "06"},
        {"MUL", "07"},
        {"DIV", "08"},
        {"COMP", "09"},
        {"JUMP", "0A"},
        {"JEQ", "0B"},
        {"JGT", "0C"},
        {"JLT", "0D"},
        {"JSUB", "0E"},
        {"RSUB", "0F"},
        {"TIX", "10"},
        {"AND", "11"},
        {"OR", "12"},
        {"SHIFTL", "13"},
        {"SHIFTR", "14"},
        {"RMO", "15"},
        {"CLEAR", "16"},
        {"LDB", "17"},
        {"STB", "18"},
        {"LDL", "19"},
        {"STL", "1A"},
        {"LDT", "1B"},
        {"STT", "1C"}
    };

    for (int i = 0; i < sizeof(opcodes) / sizeof(opcodes[0]); i++) {
        memset(optab[optabIndex].opcode, 0, MAX_OPCODE_LEN);
        memset(optab[optabIndex].mnemonic, 0, 3);
        strcpy(optab[optabIndex].opcode, opcodes[i][0]);
        strcpy(optab[optabIndex].mnemonic, opcodes[i][1]);
        optabIndex++;
    }
}

void generateIntermediateCode(const char opcode[], const char operand[]) {
    int opcodeIndex = searchOpcode(opcode);
    if (operand[0] == '=') {
        printf("IC: (%s,%s) %s\n", optab[opcodeIndex].mnemonic, optab[opcodeIndex].opcode, operand);
    } else {
        printf("IC: (%s,%s) S\n", optab[opcodeIndex].mnemonic, optab[opcodeIndex].opcode);
    }
}
