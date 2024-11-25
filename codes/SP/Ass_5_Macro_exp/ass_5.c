#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MDT_SIZE 100
#define MAX_MNT_SIZE 10
#define MAX_APT_SIZE 10
#define MAX_LINE_LENGTH 100

// Structure for the Macro Name Table (MNT)
typedef struct {
    char name[20];         // Macro name
    int mdt_index;         // MDT index pointer
    int pp;                // Number of positional parameters
    int kp;                // Number of keyword parameters
    int ev;                // Number of expansion variables
    int kpdt_index;        // Pointer to KPDTAB (Keyword Parameter Definition Table)
    int sstab_index;       // Pointer to Sequence Symbol Table (SSTAB)
} MNT;

// Structure for the Macro Definition Table (MDT)
typedef struct {
    char definition[MAX_LINE_LENGTH];  // Each line of macro definition
} MDT;

// Global Tables
MNT mnt[MAX_MNT_SIZE];     // Macro Name Table
MDT mdt[MAX_MDT_SIZE];     // Macro Definition Table
char apt[MAX_APT_SIZE][20]; // Argument Parameter Table (APT)
int mnt_count = 0, mdt_count = 0, apt_count = 0;  // Counters

// Function to add a macro name to MNT
void add_to_mnt(char *macro_name, int index, int pp, int kp, int ev, int kpdt_index, int sstab_index) {
    strcpy(mnt[mnt_count].name, macro_name);
    mnt[mnt_count].mdt_index = index;
    mnt[mnt_count].pp = pp;
    mnt[mnt_count].kp = kp;
    mnt[mnt_count].ev = ev;
    mnt[mnt_count].kpdt_index = kpdt_index;
    mnt[mnt_count].sstab_index = sstab_index;
    mnt_count++;
}

// Function to add a line to MDT
void add_to_mdt(char *definition) {
    strcpy(mdt[mdt_count].definition, definition);
    mdt_count++;
}

// Function to find a macro in MNT and return its MDT index
int find_in_mnt(char *macro_name) {
    for (int i = 0; i < mnt_count; i++) {
        if (strcmp(mnt[i].name, macro_name) == 0) {
            return i;  // Return index in MNT
        }
    }
    return -1;
}

// Function to fill the Argument Parameter Table (APT)
void fill_apt(char *args, int pp, int kp, int mnt_index) {
    apt_count = 0;
    char *token = strtok(args, ", \n");

    // Fill positional parameters
    for (int i = 0; i < pp; i++) {
        if (token != NULL) {
            strncpy(apt[apt_count++], token, 20);
            token = strtok(NULL, ", \n");
        } else {
            strncpy(apt[apt_count++], "-", 20); // Empty positional parameter
        }
    }

    // Handle keyword parameters with a default value
    for (int i = 0; i < kp; i++) {
        if (token != NULL) {
            strncpy(apt[apt_count++], token, 20);
            token = strtok(NULL, ", \n");
        } else {
            strncpy(apt[apt_count++], "DEFAULT", 20); // Default value for keyword parameter
        }
    }
}

// Function to expand macros and write to output
void expand_macro(FILE *output, int mnt_index) {
    int mdt_index = mnt[mnt_index].mdt_index;
    int pp = mnt[mnt_index].pp;
    int kp = mnt[mnt_index].kp;
    char expanded_line[MAX_LINE_LENGTH];

    for (int i = mdt_index; i < mdt_count; i++) {
        if (strncmp(mdt[i].definition, "MEND", 4) == 0) break;

        strcpy(expanded_line, mdt[i].definition);

        // Replace parameters in the macro definition
        for (int j = 0; j < pp + kp; j++) {
            char param[3] = { '&', 'A' + j, '\0' };
            char *pos = strstr(expanded_line, param);

            while (pos != NULL) {
                char buffer[MAX_LINE_LENGTH];
                int len = pos - expanded_line;

                strncpy(buffer, expanded_line, len);
                buffer[len] = '\0';

                strcat(buffer, apt[j]);
                strcat(buffer, pos + strlen(param));

                strcpy(expanded_line, buffer);
                pos = strstr(expanded_line, param);
            }
        }
        fputs(expanded_line, output);
    }
}

// Function to process Pass 1: Define Macros
void pass1(FILE *input) {
    char line[MAX_LINE_LENGTH], macro_name[20];
    int pp = 0, kp = 0, ev = 0, kpdt_index = -1, sstab_index = -1;

    while (fgets(line, sizeof(line), input)) {
        if (strncmp(line, "MACRO", 5) == 0) {
            fgets(line, sizeof(line), input);
            sscanf(line, "%s %d %d", macro_name, &pp, &kp);
            add_to_mnt(macro_name, mdt_count, pp, kp, ev, kpdt_index, sstab_index);

            while (fgets(line, sizeof(line), input)) {
                if (strncmp(line, "MEND", 4) == 0) {
                    add_to_mdt("MEND\n");
                    break;
                }
                add_to_mdt(line);
            }
        }
    }
}

// Function to process Pass 2: Expand Macros
void pass2(FILE *input, FILE *output) {
    char line[MAX_LINE_LENGTH], macro_name[20], args[MAX_LINE_LENGTH];
    int inside_macro_definition = 0;

    while (fgets(line, sizeof(line), input)) {
        sscanf(line, "%s", macro_name);

        if (strncmp(line, "MACRO", 5) == 0) {
            inside_macro_definition = 1;
            continue;
        }

        if (strncmp(line, "MEND", 4) == 0) {
            inside_macro_definition = 0;
            continue;
        }

        if (inside_macro_definition) continue;

        int mnt_index = find_in_mnt(macro_name);
        if (mnt_index != -1) {
            strcpy(args, line + strlen(macro_name));
            fill_apt(args, mnt[mnt_index].pp, mnt[mnt_index].kp, mnt_index);
            expand_macro(output, mnt_index);
        } else {
            fputs(line, output);
        }
    }
}

// Function to print MNT
void print_mnt() {
    printf("\nMacro Name Table (MNT):\n");
    printf("Index\tName\tMDT Index\t#PP\t#KP\n");
    for (int i = 0; i < mnt_count; i++) {
        printf("%d\t%s\t%d\t\t%d\t%d\n", i, mnt[i].name, mnt[i].mdt_index, mnt[i].pp, mnt[i].kp);
    }
}

// Function to print MDT
void print_mdt() {
    printf("\nMacro Definition Table (MDT):\n");
    for (int i = 0; i < mdt_count; i++) {
        printf("%d\t%s", i, mdt[i].definition);
    }
}

// Function to print APT
void print_apt() {
    printf("\nArgument Parameter Table (APT):\n");
    printf("Index\tArgument\n");
    for (int i = 0; i < apt_count; i++) {
        printf("%d\t%s\n", i, apt[i]);
    }
}

int main() {
    FILE *input = fopen("input2.txt", "r");
    FILE *output = fopen("output2.txt", "w");

    if (input == NULL || output == NULL) {
        printf("Error opening files.\n");
        return 1;
    }

    pass1(input);
    rewind(input);
    pass2(input, output);

    fclose(input);
    fclose(output);

    print_mnt();
    print_mdt();
    print_apt();

    printf("\nMacro expansion completed successfully.\n");
    return 0;
}
