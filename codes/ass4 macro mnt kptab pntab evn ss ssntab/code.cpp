#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <atomic>

class MNT
{
public:
    std::string name;
    int posParams;
    int keyParams;
    int expansionVars;
    int mdtPtr;
    int kpdtPtr;
    int sstPtr;

    MNT(const std::string &name, int posParams, int keyParams, int expansionVars, int mdtPtr, int kpdtPtr, int sstPtr)
        : name(name), posParams(posParams), keyParams(keyParams),
          expansionVars(expansionVars),
          mdtPtr(mdtPtr), kpdtPtr(kpdtPtr), sstPtr(sstPtr) {}

    std::string toString() const
    {
        return " Name=" + name + ", PP=" + std::to_string(posParams) +
               ", KP=" + std::to_string(keyParams) + ", ETVs=" +
               std::to_string(expansionVars) +
               ", MDT Ptr=" + std::to_string(mdtPtr) + ", KPDTAB Ptr=" +
               std::to_string(kpdtPtr) +
               ", SST Ptr=" + std::to_string(sstPtr);
    }
};

class PNTAB
{
public:
    std::vector<std::string> paramNames;

    void addParam(const std::string &param)
    {
        paramNames.push_back(param);
    }

    int getParamIndex(const std::string &param) const
    {
        auto it = std::find(paramNames.begin(), paramNames.end(), param);
        return (it != paramNames.end()) ? std::distance(paramNames.begin(),
                                                        it)
                                        : -1;
    }

    std::string toString() const
    {
        std::string result = " ";
        for (const auto &param : paramNames)
        {
            result += param + " ";
        }
        return result;
    }
};

class KPDTAB
{
public:
    std::string paramName;
    std::string defaultValue;

    KPDTAB(const std::string &paramName, const std::string &defaultValue)
        : paramName(paramName), defaultValue(defaultValue) {}

    std::string toString() const
    {
        return " " + paramName + " = " + defaultValue;
    }
};

class EVNTAB
{
public:
    std::vector<std::string> expansionVars;

    void addVar(const std::string &varName)
    {
        expansionVars.push_back(varName);
    }

    std::string toString() const
    {
        std::string result = " ";
        for (const auto &var : expansionVars)
        {
            result += var + " ";
        }
        return result;
    }
};

class SSTAB
{
public:
    std::string symbolName;
    int locationInMDT;

    SSTAB(const std::string &symbolName, int locationInMDT)
        : symbolName(symbolName), locationInMDT(locationInMDT) {}

    std::string toString() const
    {
        return symbolName + ": " + std::to_string(locationInMDT);
    }
};

class SSNTAB
{
public:
    std::vector<std::string> symbolNames;

    void addSymbol(const std::string &symbolName)
    {
        symbolNames.push_back(symbolName);
    }

    int getSymbolIndex(const std::string &symbolName) const
    {
        auto it = std::find(symbolNames.begin(), symbolNames.end(),
                            symbolName);
        return (it != symbolNames.end()) ? std::distance(symbolNames.begin(), it) : -1;
    }

    std::string toString() const
    {
        std::string result = " ";
        for (const auto &symbol : symbolNames)
        {
            result += symbol + " ";
        }
        return result;
    }
};

class MDT
{
public:
    std::vector<std::string> instructions;

    void addInstruction(const std::string &instruction)
    {
        instructions.push_back(instruction);
    }

    std::string toString() const
    {
        std::string result = " ";
        for (const auto &instruction : instructions)
        {
            result += instruction + "\n";
        }
        return result;
    }
};

class MacroProcessor
{
private:
    std::vector<MNT> mntList;
    std::vector<PNTAB> pntabList;
    std::vector<KPDTAB> kpdtabList;
    std::vector<EVNTAB> evntabList;
    std::vector<SSTAB> sstablist;
    std::vector<SSNTAB> ssntabList;
    MDT mdt;

public:
    void processMacroFile(const std::string &fileName)
    {
        std::ifstream file(fileName);
        if (!file.is_open())
        {
            std::cerr << "Error opening file: " << fileName << std::endl;
            return;
        }

        std::string line;
        bool isMacro = false;
        std::string macroName = "";
        PNTAB currentPNTAB;
        EVNTAB currentEVNTAB;
        SSNTAB currentSSNTAB;
        int posParams = 0;
        int keyParams = 0;
        int mdtPtr = mdt.instructions.size();
        int kpdtabIndex = kpdtabList.size();
        std::atomic<int> sstabIndex(sstablist.size());
        bool hasKeywords = false;
        int counter = 0;

        while (std::getline(file, line))
        {
            line = trim(line);
            counter++;
            if (line.substr(0, 5) == "MACRO")
            {

                isMacro = true;
                currentEVNTAB = EVNTAB();
                currentSSNTAB = SSNTAB();
                continue;
            }
            else if (line.substr(0, 4) == "MEND")
            {
                isMacro = false;
                counter--;
                MNT mnt(macroName, posParams, keyParams,
                        currentEVNTAB.expansionVars.size(), mdtPtr,
                        hasKeywords ? kpdtabIndex : -1, sstabIndex.load());
                mntList.push_back(mnt);
                mdtPtr = mdt.instructions.size();
                pntabList.push_back(currentPNTAB);
                evntabList.push_back(currentEVNTAB);
                ssntabList.push_back(currentSSNTAB);

                currentPNTAB = PNTAB();
                macroName = "";
                kpdtabIndex = kpdtabList.size();
                sstabIndex.store(sstablist.size());
                continue;
            }

            if (isMacro)
            {
                if (macroName.empty())
                {
                    counter--;
                    std::vector<std::string> parts = split(line, " ");
                    macroName = parts[0];
                    for (size_t i = 1; i < parts.size(); i++)
                    {
                        std::string par = parts[i];
                        if (par[0] == '.')
                        {
                            sstablist.push_back(SSTAB(par, counter++));
                            currentPNTAB.addParam(par);
                            posParams++;
                        }
                        std::vector<std::string> parax = split(par, ",");
                        for (const auto &param : parax)
                        {
                            if (param.find('=') != std::string::npos)
                            {
                                std::vector<std::string> keyValue =
                                    split(param, "=");
                                kpdtabList.push_back(KPDTAB(keyValue[0],
                                                            keyValue[1]));
                                currentPNTAB.addParam(keyValue[0]);
                                posParams++;
                                keyParams++;
                                hasKeywords = true;
                            }
                            else
                            {
                                currentPNTAB.addParam(param);
                                posParams++;
                            }
                        }
                    }
                }
                else if (line.substr(0, 3) == "LCL")
                {
                    std::vector<std::string> vars = split(line.substr(3),
                                                          ",");
                    std::string replacedInstruction = "LCL"; // Start the instruction with "LCL"

                    for (size_t i = 0; i < vars.size(); ++i)
                    {
                        std::string trimmedVar = trim(vars[i]);
                        currentEVNTAB.addVar(trimmedVar); // Add variable to EVNTAB

                        // Append to MDT instruction in (E, index) format
                        replacedInstruction += " (E," + std::to_string(i + 1) + ")";
                    }

                    mdt.addInstruction(replacedInstruction); // Add the complete instruction to MDT
                }
                else
                {
                    std::vector<std::string> tokens = split(line, " ");
                    std::string replacedInstruction = "";
                    std::string symbolName = "";
                    for (const auto &token : tokens)
                    {
                        if (token[0] == '.')
                        {
                            symbolName = token.substr(1);
                            currentSSNTAB.addSymbol(symbolName);
                            sstablist.push_back(SSTAB(symbolName,
                                                      counter));
                        }
                        else if (true)
                        {
                            std::vector<std::string> subTokens =
                                split(token, ",");
                            for (const auto &subToken : subTokens)
                            {
                                int index =
                                    currentPNTAB.getParamIndex(subToken);
                                if (index != -1)
                                {
                                    replacedInstruction += " (P," +
                                                           std::to_string(index + 1) + ")";
                                }
                                else if (std::find(currentEVNTAB.expansionVars.begin(),
                                                   currentEVNTAB.expansionVars.end(), subToken) !=
                                         currentEVNTAB.expansionVars.end())
                                {
                                    int evIndex =
                                        std::distance(currentEVNTAB.expansionVars.begin(),
                                                      std::find(currentEVNTAB.expansionVars.begin(),
                                                                currentEVNTAB.expansionVars.end(), subToken));
                                    replacedInstruction += " (E," +
                                                           std::to_string(evIndex + 1) + ")";
                                }
                                else
                                {
                                    replacedInstruction += subToken;
                                }
                            }
                        }
                        else
                        {
                            replacedInstruction += token;
                        }
                    }
                    mdt.addInstruction(replacedInstruction);
                }
            }
        }
        file.close();
    }

    void generateOutputFile(const std::string &fileName)
    {
        std::ofstream outputFile(fileName);

        if (!outputFile.is_open())
        {
            std::cerr << "Error opening file: " << fileName << std::endl;
            return;
        }

        // Write MNT
        outputFile << "MNT:\n";
        for (const auto &mnt : mntList)
        {
            outputFile << mnt.toString() << "\n \n";
        }

        // Write PNTAB
        outputFile << "\nPNTAB:\n";
        for (const auto &pntab : pntabList)
        {
            outputFile << pntab.toString() << "\n";
        }

        // Write KPDTAB
        outputFile << "\nKPDTAB:\n";
        for (const auto &kpdtab : kpdtabList)
        {
            outputFile << kpdtab.toString() << "\n";
        }

        // Write EVNTAB
        outputFile << "\nEVNTAB:\n";
        for (const auto &evntab : evntabList)
        {
            outputFile << evntab.toString() << "\n";
        }

        // Write SSTAB
        outputFile << "\nSSTAB:\n";
        for (const auto &sstab : sstablist)
        {
            outputFile << sstab.toString() << "\n";
        }

        // Write SSNTAB
        outputFile << "\nSSNTAB:\n";
        for (const auto &ssntab : ssntabList)
        {
            outputFile << ssntab.toString() << "\n";
        }

        // Write MDT
        outputFile << "\nMDT:\n";
        outputFile << mdt.toString();

        outputFile.close();
    }

private:
    std::string trim(const std::string &str)
    {
        size_t first = str.find_first_not_of(" \t");
        size_t last = str.find_last_not_of(" \t");
        return (first == std::string::npos) ? "" : str.substr(first, (last - first + 1));
    }

    std::vector<std::string> split(const std::string &str, const std::string &delimiter)
    {
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = str.find(delimiter);
        while (end != std::string::npos)
        {
            tokens.push_back(str.substr(start, end - start));
            start = end + delimiter.length();
            end = str.find(delimiter, start);
        }
        tokens.push_back(str.substr(start, end));
        return tokens;
    }
};

int main()
{
    MacroProcessor processor;

    processor.processMacroFile("macrocode.txt"); // Replace with your actual macro file name

    processor.generateOutputFile("output.txt"); // Consolidated output file
    return 0;
}