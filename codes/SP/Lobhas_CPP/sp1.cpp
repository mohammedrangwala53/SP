#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>

class InstructionComponents {
public:
    std::string label;
    std::string mnemonic;
    std::vector<std::string> operands;

    InstructionComponents(const std::string& label, const std::string& mnemonic, const std::vector<std::string>& operands)
        : label(label), mnemonic(mnemonic), operands(operands) {}
};

std::vector<std::string> readFile(const std::string& filePath) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    std::cout << "Debug: Read " << lines.size() << " lines from the file." << std::endl;
    return lines;
}

InstructionComponents splitInstruction(const std::string& instruction) {
    std::string label;
    std::string mnemonic;
    std::vector<std::string> operands;

    std::istringstream iss(instruction);
    std::string token;
    std::vector<std::string> tokens;

    while (iss >> token) {
        tokens.push_back(token);
    }

    size_t index = 0;
    if (!tokens.empty() && tokens[index].back() == ':') {
        label = tokens[index].substr(0, tokens[index].length() - 1);
        ++index;
    }

    if (index < tokens.size()) {
        mnemonic = tokens[index++];
    }

    while (index < tokens.size()) {
        operands.push_back(tokens[index++]);
    }

    return InstructionComponents(label, mnemonic, operands);
}

int main() {
    // Hardcoded Mnemonic Opcode Table (MOT)
    std::unordered_map<std::string, int> mot = {
        {"START", 100}, {"MOV", 200}, {"ADD", 201}, {"SUB", 202},
        {"MUL", 203}, {"DIV", 204}, {"JMP", 205}, {"JNZ", 206},
        {"JZ", 207}, {"JNC", 208}, {"JC", 209}, {"HALT", 210},
        {"END", 300}, {"DC", 400}, {"DS", 401}
    };

    // Symbol Table
    std::unordered_map<std::string, int> symbolTable;

    // Intermediate Code
    std::vector<std::string> intermediateCode;

    std::string filePath = "input1.txt"; // Ensure this file exists

    try {
        std::vector<std::string> lines = readFile(filePath);

        if (lines.empty()) {
            std::cerr << "Error: No instructions found in the file." << std::endl;
            return 1;
        }

        int addressCounter = 0; // Address counter for symbol table
        for (const auto& line : lines) {
            std::cout << "Debug: Processing line: " << line << std::endl;
            InstructionComponents components = splitInstruction(line);
            std::cout << "Instruction: " << line << std::endl;
            std::cout << "Label: " << components.label << std::endl;
            std::cout << "Mnemonic: " << components.mnemonic << std::endl;

            // Add to symbol table if label exists
            if (!components.label.empty()) {
                symbolTable[components.label] = addressCounter;
            }

            // Check if the mnemonic is in the MOT table and print its value
            auto it = mot.find(components.mnemonic);
            if (it != mot.end()) {
                std::cout << "MOT Value: " << it->second << std::endl;
            } else {
                std::cout << "Mnemonic not found in MOT table." << std::endl;
            }

            std::cout << "Operands: ";
            for (const auto& operand : components.operands) {
                std::cout << operand << " ";
            }
            std::cout << std::endl << std::endl;

            // Generate intermediate code representation
            std::ostringstream intermediateLine;
            intermediateLine << addressCounter << ": ";
            if (!components.label.empty()) {
                intermediateLine << components.label << ": ";
            }
            intermediateLine << components.mnemonic << " ";
            for (const auto& operand : components.operands) {
                intermediateLine << operand << " ";
            }

            intermediateCode.push_back(intermediateLine.str());

            // Increment the address counter
            addressCounter++;
        }

        // Output the symbol table
        std::cout << "Symbol Table:" << std::endl;
        if (symbolTable.empty()) {
            std::cout << "No symbols found." << std::endl;
        } else {
            for (const auto& entry : symbolTable) {
                const auto& label = entry.first;
                const auto& address = entry.second;
                std::cout << "Label: " << label << ", Address: " << address << std::endl;
            }
        }

        // Output the intermediate code
        std::cout << "Intermediate Code:" << std::endl;
        for (const auto& line : intermediateCode) {
            std::cout << line << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}






   