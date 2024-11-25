import java.util.HashMap;
import java.util.Map;
import java.util.ArrayList;
import java.util.List;

public class Pass2Assembler {

    // Macro Operation Table (MOT)
    static class MOTEntry {
        String opcode;
        int operands;
        int size;
        String type;

        MOTEntry(String opcode, int operands, int size, String type) {
            this.opcode = opcode;
            this.operands = operands;
            this.size = size;
            this.type = type;
        }
    }

    static Map<String, MOTEntry> MOT = new HashMap<>() {{
        put("20", new MOTEntry("20", 2, 2, "LS"));
        put("21", new MOTEntry("21", 1, 2, "LS"));
        put("22", new MOTEntry("22", 1, 3, "LS"));
        put("23", new MOTEntry("23", 1, 2, "LS"));
        put("24", new MOTEntry("24", 1, 2, "LS"));
        put("25", new MOTEntry("25", 1, 2, "LS"));
        put("26", new MOTEntry("26", 0, 0, "AD"));
        put("27", new MOTEntry("27", 0, 0, "AD"));
        put("01", new MOTEntry("01", 0, 0, "DL"));
        put("02", new MOTEntry("02", 0, 0, "DL"));
        put("1", new MOTEntry("1", 0, 0, "R"));
        put("2", new MOTEntry("2", 0, 0, "R"));
        put("3", new MOTEntry("3", 0, 0, "R"));
        put("4", new MOTEntry("4", 0, 0, "R"));
    }};

    // Symbol Table
    static Map<Integer, Integer> symbolTable = new HashMap<>() {{
        put(0, 106);  // x
        put(1, 107);  // y
    }};

    // Intermediate Code (IC)
    static List<String> IC = new ArrayList<>() {{
        add("LC: 100 (AD,01) (C,100)");
        add("LC: 100 (LS,20) (R,1) (R,2)");
        add("LC: 102 (LS,21) (R,3)");
        add("LC: 104 (LS,23) (R,1)");
        add("LC: 106 (DL,01) (C,x)");
        add("LC: 107 (DL,02) (C,10)");
        add("LC: 117 (LS,22) (ST,0)");
        add("LC: 120 (AD,27)");
    }};

    // Function to generate machine code from IC
    public static List<String> generateMachineCode() {
        List<String> machineCode = new ArrayList<>();
        for (String line : IC) {
            String[] parts = line.split(" ");
            String locationCounter = parts[1];
            StringBuilder code = new StringBuilder(locationCounter + " ");
            for (int i = 2; i < parts.length; i++) {
                String[] token = parts[i].replaceAll("[()]", "").split(",");
                String type = token[0];
                String value = token[1];
                switch (type) {
                    case "AD":
                    case "DL":
                    case "LS":
                    case "R":
                        if (MOT.containsKey(value)) {
                            code.append(MOT.get(value).opcode).append(" ");
                        } else {
                            System.err.println("Error: Opcode not found for value " + value);
                        }
                        break;
                    case "C": // Constant
                        if (value.matches("\\d+")) {
                            code.append(value).append(" ");
                        } else if (value.matches("[a-zA-Z]")) { // Symbolic constant (e.g., 'x')
                            int symbolAddress = symbolTable.get(value.charAt(0) - 'x');
                            code.append(symbolAddress).append(" ");
                        }
                        break;
                    case "ST": // Symbol
                        if (symbolTable.containsKey(Integer.parseInt(value))) {
                            code.append(symbolTable.get(Integer.parseInt(value))).append(" ");
                        } else {
                            System.err.println("Error: Symbol not found for value " + value);
                        }
                        break;
                    default:
                        System.err.println("Error: Unrecognized type " + type);
                }
            }
            machineCode.add(code.toString().trim());
        }
        return machineCode;
    }

    public static void main(String[] args) {
        List<String> machineCode = generateMachineCode();
        // Output the generated machine code
        System.out.println("Generated Machine Code:");
        for (String code : machineCode) {
            System.out.println(code);
        }
    }
}
