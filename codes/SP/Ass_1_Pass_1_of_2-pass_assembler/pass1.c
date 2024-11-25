#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void passOne(char label[], char opcode[], char operand[], char code[], char mnemonic[]);
void display();

int main()
{
    char label[10], opcode[10], operand[10];
    char code[10], mnemonic[3];
    passOne(label, opcode, operand, code, mnemonic);
    return 0;
}

void passOne(char label[], char opcode[], char operand[], char code[], char mnemonic[])
{
    int locctr, start, length;
    FILE *inputFile, *optabFile, *symtabFile, *intermediateFile, *lengthFile;

    inputFile = fopen("input.txt", "r");
    optabFile = fopen("optab.txt", "r");
    symtabFile = fopen("symtab.txt", "w");
    intermediateFile = fopen("intermediate.txt", "w");
    lengthFile = fopen("length.txt", "w");

    if (!inputFile || !optabFile || !symtabFile || !intermediateFile || !lengthFile) {
        printf("Error opening file(s).\n");
        return;
    }

    // Print column headings
    fprintf(intermediateFile, "LocCtr\tLabel\tOpcode\tOperand\n");
    fprintf(symtabFile, "Label\tLocCtr\n");

    fscanf(inputFile, "%s\t%s\t%s", label, opcode, operand);

    if (strcmp(opcode, "START") == 0)
    {
        start = atoi(operand);
        locctr = start;
        fprintf(intermediateFile, "\t%s\t%s\t%s\n", label, opcode, operand);
        fscanf(inputFile, "%s\t%s\t%s", label, opcode, operand);
    }
    else
    {
        locctr = 0;
    }

    while (strcmp(opcode, "END") != 0)
    {
        fprintf(intermediateFile, "%d\t%s\t%s\t%s\n", locctr, label, opcode, operand);

        if (strcmp(label, "**") != 0)
        {
            fprintf(symtabFile, "%s\t%d\n", label, locctr);
        }

        int found = 0;
        fseek(optabFile, 0, SEEK_SET);
        while (fscanf(optabFile, "%s\t%s", code, mnemonic) != EOF)
        {
            if (strcmp(opcode, code) == 0)
            {
                locctr += 3;
                found = 1;
                break;
            }
        }

        if (!found)
        {
            if (strcmp(opcode, "WORD") == 0)
            {
                locctr += 3;
            }
            else if (strcmp(opcode, "RESW") == 0)
            {
                locctr += (3 * atoi(operand));
            }
            else if (strcmp(opcode, "BYTE") == 0)
            {
                locctr += strlen(operand) - 3; // Assuming 'BYTE C'' format
            }
            else if (strcmp(opcode, "RESB") == 0)
            {
                locctr += atoi(operand);
            }
            else if (strcmp(opcode, "DS") == 0)
            {
                locctr += atoi(operand);
            }
            else if (strcmp(opcode, "DC") == 0)
            {
                locctr += 1;
            }
            else
            {
                printf("Error: Invalid opcode '%s' at locctr %d\n", opcode, locctr);
                fprintf(intermediateFile, "%d\t%s\t%s\t%s\t%s\n", locctr, label, opcode, operand, "ERROR: Invalid opcode");
                fclose(inputFile);
                fclose(optabFile);
                fclose(symtabFile);
                fclose(intermediateFile);
                fclose(lengthFile);
                return;
            }
        }

        fscanf(inputFile, "%s\t%s\t%s", label, opcode, operand);
    }

    fprintf(intermediateFile, "%d\t%s\t%s\t%s\n", locctr, label, opcode, operand);

    fclose(inputFile);
    fclose(optabFile);
    fclose(symtabFile);
    fclose(intermediateFile);

    display();

    length = locctr - start;
    fprintf(lengthFile, "%d", length);
    fclose(lengthFile);
    printf("\nThe length of the code : %d\n", length);
}

void display()
{
    char ch;
    FILE *file;

    printf("\nThe contents of Input Table :\n\n");
    file = fopen("input.txt", "r");
    if (file)
    {
        while ((ch = fgetc(file)) != EOF)
        {
            putchar(ch);
        }
        fclose(file);
    }
    else
    {
        printf("Error opening input file.\n");
    }

    printf("\n\nThe contents of Output Table :\n\n");
    file = fopen("intermediate.txt", "r");
    if (file)
    {
        while ((ch = fgetc(file)) != EOF)
        {
            putchar(ch);
        }
        fclose(file);
    }
    else
    {
        printf("Error opening intermediate file.\n");
    }

    printf("\n\nThe contents of Symbol Table :\n\n");
    file = fopen("symtab.txt", "r");
    if (file)
    {
        while ((ch = fgetc(file)) != EOF)
        {
            putchar(ch);
        }
        fclose(file);
    }
    else
    {
        printf("Error opening symbol table file.\n");
    }
}
