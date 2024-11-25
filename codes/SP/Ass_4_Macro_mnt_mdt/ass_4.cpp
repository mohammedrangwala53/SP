#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>  // For std::find
#include <iomanip>    // For std::setw

using namespace std;

struct Macro {
    string name;
    vector<string> parameters;
    vector<string> body;
};

struct MNTEntry {
    int index;
    string name;
    int mdtIndex;
    int kpdtIndex;
    int paramCount;
};

struct Tables {
    vector<MNTEntry> MNT;               // Macro Name Table with additional columns
    vector<string> MDT;                 // Macro Definition Table
    map<string, vector<string>> PNTAB;  // Parameter Name Table
    map<string, vector<string>> KPDTAB; // Keyword Parameter Table
    vector<string> EVNTAB;              // Expansion Variable Name Table
    vector<string> SSNTAB;              // Sequence Number Table
    vector<string> SSTAB;               // Sequence Symbol Table
};

// Check if macro already exists in MNT
bool macroExists(const vector<MNTEntry>& MNT, const string& macroName) {
    for (const auto& entry : MNT) {
        if (entry.name == macroName) return true;
    }
    return false;
}

// Process the macro definition and store it
void processMacroDefinition(ifstream& inputFile, Tables& tables, const string& macroName) {
    int mdtIndex = tables.MDT.size();       // Starting index of macro in MDT
    int kpdtIndex = tables.KPDTAB.size();   // Starting index for KPDTAB

    // Create a new entry in MNT
    MNTEntry newEntry = {static_cast<int>(tables.MNT.size() + 1), macroName, mdtIndex, kpdtIndex, 0};
    tables.MNT.push_back(newEntry);

    string line;
    vector<string> positionalParams;
    vector<string> keywordParams;

    // Read parameters (including keyword parameters)
    if (getline(inputFile, line)) {
        stringstream ss(line);
        string param;
        while (ss >> param) {
            if (param.find('=') != string::npos) {
                // Keyword parameter with default value
                keywordParams.push_back(param);
            } else {
                // Positional parameter
                positionalParams.push_back(param);
            }
        }
        // Store in PNTAB and KPDTAB
        tables.PNTAB[macroName] = positionalParams;
        tables.KPDTAB[macroName] = keywordParams;

        // Update the parameter count in MNT entry
        tables.MNT.back().paramCount = positionalParams.size() + keywordParams.size();

        // Add all parameters (positional and keyword) to EVNTAB
        tables.EVNTAB.insert(tables.EVNTAB.end(), positionalParams.begin(), positionalParams.end());
        tables.EVNTAB.insert(tables.EVNTAB.end(), keywordParams.begin(), keywordParams.end());
    }

    // Read macro body
    int seqNum = 1;
    while (getline(inputFile, line)) {
        if (line == "MEND") {
            tables.MDT.push_back("MEND");
            break;
        }
        tables.MDT.push_back(line);

        // Add sequence numbers and symbols to SSNTAB and SSTAB
        stringstream bodyStream(line);
        string symbol;
        if (bodyStream >> symbol) {
            tables.SSNTAB.push_back(to_string(seqNum++));
            tables.SSTAB.push_back(symbol);
            if (find(tables.EVNTAB.begin(), tables.EVNTAB.end(), symbol) == tables.EVNTAB.end()) {
                tables.EVNTAB.push_back(symbol);  // Add symbol if not already present
            }
        }
    }
}

// Detect duplicate macros in MNT
void detectDuplicateMacro(const vector<MNTEntry>& MNT) {
    map<string, int> macroCount;
    for (const auto& entry : MNT) {
        macroCount[entry.name]++;
        if (macroCount[entry.name] > 1) {
            cout << "Error: Duplicate macro definition detected for " << entry.name << endl;
        }
    }
}

// Write the tables to a file
void writeToFile(const Tables& tables, const string& fileName) {
    ofstream outputFile(fileName);

    if (!outputFile.is_open()) {
        cerr << "Error opening output file." << endl;
        return;
    }

    // Writing MNT
    outputFile << "MNT (Macro Name Table):" << endl;
    outputFile << "+-------+------------+-----------+--------------+-------------+" << endl;
    outputFile << "| Index | Macro Name | MDT Index | KPDTAB Index | Param Count |" << endl;
    outputFile << "+-------+------------+-----------+--------------+-------------+" << endl;

    for (const auto& entry : tables.MNT) {
        outputFile << "| " << setw(5) << entry.index << " | "
                   << setw(10) << entry.name << " | "
                   << setw(9) << entry.mdtIndex << " | "
                   << setw(12) << entry.kpdtIndex << " | "
                   << setw(11) << entry.paramCount << " |" << endl;
    }
    outputFile << "+-------+------------+-----------+--------------+-------------+" << endl;

    // Writing MDT
    outputFile << "\nMDT (Macro Definition Table):" << endl;
    for (const auto& definition : tables.MDT) {
        outputFile << definition << endl;
    }

    // Writing PNTAB
    outputFile << "\nPNTAB (Parameter Name Table):" << endl;
    for (const auto& entry : tables.PNTAB) {
        outputFile << entry.first << ": ";
        for (const auto& param : entry.second) {
            outputFile << param << " ";
        }
        outputFile << endl;
    }

    // Writing KPDTAB
    outputFile << "\nKPDTAB (Keyword Parameter Table):" << endl;
    for (const auto& entry : tables.KPDTAB) {
        outputFile << entry.first << ": ";
        for (const auto& param : entry.second) {
            outputFile << param << " ";
        }
        outputFile << endl;
    }

    // Writing EVNTAB
    outputFile << "\nEVNTAB (Expansion Variable Name Table):" << endl;
    for (const auto& entry : tables.EVNTAB) {
        outputFile << entry << endl;
    }

    // Writing SSNTAB
    outputFile << "\nSSNTAB (Sequence Number Table):" << endl;
    for (const auto& entry : tables.SSNTAB) {
        outputFile << entry << endl;
    }

    // Writing SSTAB
    outputFile << "\nSSTAB (Sequence Symbol Table):" << endl;
    for (const auto& entry : tables.SSTAB) {
        outputFile << entry << endl;
    }

    outputFile.close();
    cout << "Output successfully created in file: " << fileName << endl;
}

int main() {
    ifstream inputFile("input.txt");  // Input file
    if (!inputFile.is_open()) {
        cerr << "Error opening file" << endl;
        return 1;
    }

    string line;
    Tables tables;

    // Process input line by line
    while (getline(inputFile, line)) {
        if (line.find("MACRO") != string::npos) {
            getline(inputFile, line); // Get macro name line
            stringstream ss(line);
            string macroName;
            ss >> macroName;

            // Check for duplicate macro definitions
            if (macroExists(tables.MNT, macroName)) {
                cout << "Error: Duplicate macro definition for " << macroName << endl;
                continue;
            }

            processMacroDefinition(inputFile, tables, macroName);
        }
    }

    inputFile.close();

    // Detect duplicates in MNT
    detectDuplicateMacro(tables.MNT);
    writeToFile(tables, "output.txt");

    return 0;
}
