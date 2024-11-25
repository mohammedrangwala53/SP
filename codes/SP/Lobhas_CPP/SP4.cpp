#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
using namespace std;

// Data structures
struct MNTEntry {
    string macro_name;
    int MDT_index;
    int PNTAB_index;
    int KPDTAB_index;
    int EVNTAB_index;
};

struct MDTEntry {
    int index;
    string definition;
};

// Global tables
vector<MNTEntry> MNT;
vector<MDTEntry> MDT;

// Function to process input file and create the tables
void processMacroDefinition(const string& inputFile, vector<string>& codeLines) {
    ifstream file(inputFile);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << inputFile << endl;
        return;
    }

    string line;
    bool inMacro = false;
    int MDT_index = 0;

    while (getline(file, line)) {
        // Store main code lines
        if (!inMacro && !line.empty() && line.find("MACRO") == string::npos) {
            codeLines.push_back(line);
            continue; // Skip to the next line
        }

        if (line.find("MACRO") != string::npos) {
            inMacro = true;
            MNTEntry mntEntry;
            string macroName;
            file >> macroName;  // Read the macro name
            mntEntry.macro_name = macroName;
            mntEntry.MDT_index = MDT_index;
            MNT.push_back(mntEntry);
            getline(file, line);  // Read the rest of the line
        } else if (line.find("MEND") != string::npos) {
            inMacro = false;
        } else if (inMacro && !line.empty()) {
            MDTEntry mdtEntry;
            mdtEntry.index = MDT_index++;
            mdtEntry.definition = line;
            MDT.push_back(mdtEntry);
        }
    }
}

// Function to detect errors (like undefined macros)
void detectErrors(const vector<string>& codeLines) {
    for (const auto& mntEntry : MNT) {
        bool found = false;
        for (const auto& line : codeLines) {
            if (line.find(mntEntry.macro_name) != string::npos) {
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Error: Undefined macro " << mntEntry.macro_name << endl;
        }
    }
}

// Function to display tables
void displayTables() {
    cout << "\nMNT (Macro Name Table):\n";
    for (const auto& entry : MNT) {
        cout << "Macro: " << entry.macro_name << ", MDT Index: " << entry.MDT_index << endl;
    }
    cout << "\nMDT (Macro Definition Table):\n";
    for (const auto& entry : MDT) {
        cout << "Index: " << entry.index << ", Definition: " << entry.definition << endl;
    }
}

int main() {
    string inputFile = "input4.txt";
    vector<string> codeLines; // To store the main code lines
    processMacroDefinition(inputFile, codeLines);
    detectErrors(codeLines);
    displayTables();
    return 0;
}









