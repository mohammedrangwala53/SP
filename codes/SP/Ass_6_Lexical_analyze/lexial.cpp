#include<bits/stdc++.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

using namespace std;

unordered_map<string, int> keywordTokens = {
    {"auto", 1}, {"break", 2}, {"case", 3}, {"char", 4}, {"const", 5}, {"continue", 6}, {"default", 7},
    {"do", 8}, {"double", 9}, {"else", 10}, {"enum", 11}, {"extern", 12}, {"float", 13}, {"for", 14}, {"goto", 15},
    {"if", 16}, {"int", 17}, {"long", 18}, {"register", 19}, {"return", 20}, {"short", 21}, {"signed", 22},
    {"sizeof", 23}, {"static", 24}, {"struct", 25}, {"switch", 26}, {"typedef", 27}, {"union", 28},
    {"unsigned", 29}, {"void", 30}, {"volatile", 31}, {"while", 32}
};

unordered_map<string, int> operatorTokens = {
    {"+", 33}, {"-", 34}, {"*", 35}, {"/", 36}, {"=", 37}, {"==", 38}, {"!=", 39}, {">", 40}, 
    {">=", 41}, {"<", 42}, {"<=", 43}
};

unordered_map<char, int> symbolTokens = {
    {',', 44}, {';', 45}, {'(', 46}, {')', 47}, {'{', 48}, {'}', 49}, {'[', 50}, {']', 51}
};

int getKeywordToken(const string &buffer) {
    if (keywordTokens.find(buffer) != keywordTokens.end())
        return keywordTokens[buffer];
    return -1; 
}

bool isValidIdentifier(const string &str) {
    if (!isalpha(str[0]) && str[0] != '_')
        return false;
    for (char ch : str) {
        if (!isalnum(ch) && ch != '_')
            return false;
    }
    return true;
}

int main() {
    ifstream fin("lexicalinput.txt");
    if (!fin.is_open()) {
        cout << "Error while opening the file\n";
        exit(0);
    }

    char ch;
    string buffer;
    int lineNo = 1;

    
    cout << "┌────────────┬───────────────┬──────────────┬───────────────────────┐\n";
    cout << "│ " << setw(10) << "Line No" << " │ " << setw(13) << "Lexeme" 
         << " │ " << setw(12) << "Token" << " │ " << setw(18) << "Token_Value" << " │\n";
    cout << "├────────────┼───────────────┼──────────────┼───────────────────────┤\n";

    while (fin.get(ch)) {
        if (ch == '\n') {
            lineNo++;
        }

        if (isspace(ch)) {
            continue;
        }

        
        if (isalpha(ch) || ch == '_') {
            buffer = ch;
            while (fin.get(ch) && (isalnum(ch) || ch == '_')) {
                buffer += ch;
            }
            fin.putback(ch);

            int keywordToken = getKeywordToken(buffer);
            if (keywordToken != -1) {
                cout << "│ " << setw(10) << lineNo << " │ " << setw(13) << buffer 
                     << " │ " << setw(12) << "Keyword" << " │ " << setw(18) << keywordToken << " │\n";
            } else if (isValidIdentifier(buffer)) {
                cout << "│ " << setw(10) << lineNo << " │ " << setw(13) << buffer 
                     << " │ " << setw(12) << "Identifier" << " │ " << setw(18) << "52" << " │\n"; 
            } else {
                cout << "│ " << setw(10) << lineNo << " │ " << setw(13) << buffer 
                     << " │ " << setw(12) << "Error" << " │ " << setw(18) << "Invalid Identifier" << " │\n";
            }
        }

        
        else if (isdigit(ch)) {
            buffer = ch;
            while (fin.get(ch) && (isdigit(ch) || ch == '.')) {
                buffer += ch;
            }
            fin.putback(ch);
            cout << "│ " << setw(10) << lineNo << " │ " << setw(13) << buffer 
                 << " │ " << setw(12) << "Numeric" << " │ " << setw(18) << "53" << " │\n";
        }

        
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' ||
                 ch == '>' || ch == '<' || ch == '!') {
            string op(1, ch);
            if ((ch == '=' || ch == '>' || ch == '<' || ch == '!') && fin.peek() == '=') {
                fin.get(ch);
                op += ch;
            }
            int operatorToken = operatorTokens[op];
            cout << "│ " << setw(10) << lineNo << " │ " << setw(13) << op 
                 << " │ " << setw(12) << "Operator" << " │ " << setw(18) << operatorToken << " │\n";
        }

    
        else if (symbolTokens.find(ch) != symbolTokens.end()) {
            cout << "│ " << setw(10) << lineNo << " │ " << setw(13) << ch 
                 << " │ " << setw(12) << "Symbol" << " │ " << setw(18) << symbolTokens[ch] << " │\n";
        }
    }


    cout << "└────────────┴───────────────┴──────────────┴─────────────────────┘\n";

    fin.close();
    return 0;
}
