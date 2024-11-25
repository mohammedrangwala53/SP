import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class a1 {

    public static void main(String[] args) {
        String[][] MOT = {
                {"move", "20", "2", "2","LS"}, // mnemonic, opcode, operands, size, type
                {"add", "21", "1", "2","LS"},    // single operand
                {"jump", "22", "1", "3","LS"},   // single operand
                {"sub", "23", "1", "2","LS"},    // single operand
                {"mul", "24", "1", "2","LS"},    // single operand
                {"div", "25", "1", "2","LS"},    // single operand
                {"start", "26", "0", "0","AD"},
                {"end", "27", "0", "0","AD"},
                {"dc", "1", "0", "0","DL"},
                {"ds", "2", "0", "0","DL"},
                {"a", "1", "0", "0","R"},
                {"b", "2", "0", "0","R"},
                {"c", "3", "0", "0","R"},
                {"d", "4", "0", "0","R"}
        };

        HashMap<String, String[]> MOTMap = new HashMap<>();
        for (String[] instruction : MOT) {
            MOTMap.put(instruction[0], new String[]{instruction[1], instruction[2], instruction[3], instruction[4]});
        }

        String[][] pass1 = new String[100][10]; // Increased size for handling more instructions
        HashMap<String, Integer> symbolTable = new HashMap<>();
        List<String> symbolList = new ArrayList<>();
        int startAddress = 0;
        int currentAddress = 0;
        int instructionCount = 0;
        boolean startFound = false;

        // Assembly code
        String[] assemblyProgram = {
                "start 100",
                "move a, b",
                "add c, d",
                "sub a, 5",
                "dc x",
                "ds y 10",
                "jump x",
                "end"
        };

        for (String line : assemblyProgram) {
            if (line.contains(":")) {
                String[] val = line.split(":");
                pass1[instructionCount][0] = val[0].trim();
                if (!symbolTable.containsKey(val[0].trim())) {
                    symbolTable.put(val[0].trim(), currentAddress); // record label address
                    symbolList.add(val[0].trim());
                }
                line = val[1].trim();  // process the remaining part after the label
            }

            String[] parts = line.trim().split(" ", 2);
            String mnemonic = parts[0];
            if (!MOTMap.containsKey(mnemonic)) {
                System.err.println("Error: Invalid mnemonic '" + mnemonic + "' found in the input.");
                return;
            }
            pass1[instructionCount][1] = mnemonic;  // mnemonic

            if (mnemonic.equals("start")) {
                if (parts.length > 1) {
                    // If there's an address, use it
                    currentAddress = Integer.parseInt(parts[1].trim());
                    startAddress = currentAddress;
                } else {
                    // Default address is 0 if not specified
                    currentAddress = 0;
                }
                startFound = true;
            } else if (mnemonic.equals("end")) {
                // For now, we just recognize 'end' without special processing
                continue; // Skip further processing for this line
            }

            if (parts.length > 1) {
                String operands = parts[1].trim();
                if (operands.contains(",")) {
                    String[] operandParts = operands.split(",");
                    for (int i = 0; i < operandParts.length; i++) {
                        pass1[instructionCount][2 + i] = operandParts[i].trim();
                    }
                } else if (operands.contains(" ")) {
                    String[] operandParts = operands.split(" ");
                    pass1[instructionCount][2] = operandParts[1].trim();
                } else {
                    pass1[instructionCount][2] = operands;  // single operand
                }
            }

            // Process Define Constant (DC) and Define Storage (DS)
            if (mnemonic.equals("dc")) {
                String[] operands = parts[1].trim().split(" ");
                String symbol = operands[0];
                if (!symbolTable.containsKey(symbol)) {
                    symbolTable.put(symbol, currentAddress);
                    symbolList.add(symbol);
                }
                currentAddress++;  // Move address by 1 for each constant
            } else if (mnemonic.equals("ds")) {
                String[] operands = parts[1].trim().split(" ");
                String symbol = operands[0];
                int size = Integer.parseInt(operands[1]);
                if (!symbolTable.containsKey(symbol)) {
                    symbolTable.put(symbol, currentAddress);
                    symbolList.add(symbol);
                }
                currentAddress += size;  // Move address by size for storage
            } else if (MOTMap.containsKey(mnemonic)) {
                String[] motEntry = MOTMap.get(mnemonic);
                currentAddress += Integer.parseInt(motEntry[2]);
            }

            instructionCount++;
        }

        if (!startFound) {
            System.out.println("Error: No 'start' instruction found in the input.");
            return;
        }

        // Generate and print Intermediate Code
        System.out.println("Intermediate Code:");
        for (int i = 0; i < pass1.length; i++) {
            if (pass1[i][1] == null) break;

            String mnemonic = pass1[i][1];
            switch (mnemonic) {
                case "start" -> {
                    System.out.print("LC: " + startAddress + " ");
                    System.out.print("(AD,01) (C," + startAddress + ")\n");
                    continue;
                }
                case "dc" -> {
                    System.out.print("LC: " + startAddress + " ");
                    System.out.print("(DL,01) (C," + pass1[i][2] + ")\n");
                    startAddress++;
                    continue;
                }
                case "ds" -> {
                    System.out.print("LC: " + startAddress + " ");
                    System.out.print("(DL,02) (C," + pass1[i][2] + ")\n");
                    startAddress += Integer.parseInt(pass1[i][2]);
                    continue;
                }
            }

            if (MOTMap.containsKey(mnemonic)) {
                String[] motEntry = MOTMap.get(mnemonic);
                String opcode = motEntry[0];
                String operandsCount = motEntry[1];

                System.out.print("LC: " + startAddress + " ");
                startAddress += Integer.parseInt(motEntry[2]);

                // Print opcode
                System.out.print("(" + motEntry[3] + "," + opcode + ") ");

                // Process operands
                if (operandsCount.equals("2")) { // If it has two operands
                    for (int j = 2; j < 4; j++) {
                        String operand = pass1[i][j];
                        if (MOTMap.containsKey(operand)) {
                            System.out.print("("+MOTMap.get(operand)[3] +"," + MOTMap.get(operand)[0] + ") ");
                        } else if (operand != null) {
                            try {
                                int operandVal = Integer.parseInt(operand);
                                System.out.print("(C," + operandVal + ") ");
                            } catch (NumberFormatException e) {
                                if (symbolTable.containsKey(operand)) {
                                    System.out.print("(ST," + symbolList.indexOf(operand) + ") ");
                                } else {
                                    System.err.println("Invalid Operand/Not Present in Symbol Table - " + operand);
                                }
                            }
                        }
                    }
                } else if (operandsCount.equals("1")) { // If it has one operand
                    String operand = pass1[i][2];
                    if (MOTMap.containsKey(operand)) {
                        System.out.print("("+MOTMap.get(operand)[3] +"," + MOTMap.get(operand)[0] + ") ");
                    } else if (operand != null) {
                        try {
                            int operandVal = Integer.parseInt(operand);
                            System.out.print("(C," + operandVal + ") ");
                        } catch (NumberFormatException e) {
                            if (symbolTable.containsKey(operand)) {
                                System.out.print("(ST," + symbolList.indexOf(operand) + ") ");
                            } else {
                                System.err.println("Invalid Operand/Not Present in Symbol Table - " + operand);
                            }
                        }
                    }
                } else if (operandsCount.equals("0")) { // No operand
                    // No additional operands to process
                }
                System.out.print("\n");
            }
        }

        // Print symbol table
        System.out.println("\nSymbol Table:");
        for (String label : symbolList) {
            System.out.println(symbolList.indexOf(label) + " : " + label + " - " + symbolTable.get(label));
        }
    }
}
