#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>

using namespace std;

enum TokenType {
    T_INT, T_ID, T_NUM,T_FLOAT, T_IF, T_ELSE, T_RETURN, 
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,  
    T_SEMICOLON, T_GT, T_EOF, 
};


struct Token {
    TokenType type;
    string value;
    int line;
};

class Lexer {

    private:
        string src;
        size_t pos;
        int line;
        std::map<std::string, TokenType> symbolTable; // Variable name to type mapping
        /*
        It hold positive values. 
        In C++, size_t is an unsigned integer data type used to represent the 
        size of objects in bytes or indices, especially when working with memory-related 
        functions, arrays, and containers like vector or string. You can also use the int data type but size_t is recommended one
        */

    public:
        Lexer(const string &src) {
            this->src = src;  
            this->pos = 0; 
            this->line = 1;   
        }

        vector<Token> tokenize() {
            vector<Token> tokens;
            while (pos < src.size()) {
                char current = src[pos];
                
                if (current == '\n') {  // give error on bases of the new line. New line handling
                line++;
                pos++;
                continue;
                }
                if (isspace(current)) {
                    pos++;
                    continue;
                }
                if (isdigit(current)) {
                    tokens.push_back(Token{T_NUM, consumeNumber(), line});
                    continue;
                }
                if (isalpha(current)) {
                    string word = consumeWord();
                    if (word == "int") tokens.push_back(Token{T_INT, word, line});
                    else if (word == "float") tokens.push_back(Token{T_FLOAT, word, line});
                    else if (word == "if") tokens.push_back(Token{T_IF, word, line});
                    else if (word == "else") tokens.push_back(Token{T_ELSE, word, line});
                    else if (word == "return") tokens.push_back(Token{T_RETURN, word, line});
                    else tokens.push_back(Token{T_ID, word, line});
                    continue;
                }
                
                switch (current) {
                        case '=': tokens.push_back(Token{T_ASSIGN, "=", line}); break;
                        case '+': tokens.push_back(Token{T_PLUS, "+", line}); break;
                        case '-': tokens.push_back(Token{T_MINUS, "-", line}); break;
                        case '*': tokens.push_back(Token{T_MUL, "*", line}); break;
                        case '/': tokens.push_back(Token{T_DIV, "/", line}); break;
                        case '(': tokens.push_back(Token{T_LPAREN, "(", line}); break;
                        case ')': tokens.push_back(Token{T_RPAREN, ")", line}); break;
                        case '{': tokens.push_back(Token{T_LBRACE, "{", line}); break;  
                        case '}': tokens.push_back(Token{T_RBRACE, "}", line}); break;  
                        case ';': tokens.push_back(Token{T_SEMICOLON, ";", line}); break;
                        case '>': tokens.push_back(Token{T_GT, ">", line}); break;
                        default: 
                        cout << "Unexpected character: " << current << " on line " << line << endl;
                        exit(1);
                }
                pos++;
            }
            tokens.push_back(Token{T_EOF, "", line});
            return tokens;
        }


        string consumeNumber() {
    size_t start = pos;
    bool hasDot = false;

    while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.')) {
        if (src[pos] == '.') {
            if (hasDot) break; // Stop if there's more than one dot
            hasDot = true;
        }
        pos++;
    }

    return src.substr(start, pos - start);
}

        string consumeWord() {
            size_t start = pos;
            while (pos < src.size() && isalnum(src[pos])) pos++;
            return src.substr(start, pos - start);
        }
};



class Parser {
public:
    Parser(const vector<Token> &tokens) : tokens(tokens), pos(0) {}

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No syntax errors." << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;
    std::map<std::string, TokenType> symbolTable;

    void parseStatement() {
    if (tokens[pos].type == T_INT) {
        parseDeclaration();
    } else if (tokens[pos].type == T_ID) {
        parseAssignment();
    } else if (tokens[pos].type == T_IF) {
        parseIfStatement();
    } else if (tokens[pos].type == T_RETURN) {
        parseReturnStatement();
    } else if (tokens[pos].type == T_LBRACE) {  
        parseBlock();
    } else {
        cout << "Syntax error on line " << tokens[pos].line 
             << ": unexpected token '" << tokens[pos].value 
             << "' (" << tokenTypeToString(tokens[pos].type) << ")" << endl;
        exit(1);
    }
}
    string tokenTypeToString(TokenType type) {
    switch (type) {
        case T_ID: return "identifier";
        case T_INT: return "int";
         case T_FLOAT: return "float"; 
        case T_NUM: return "number";
        case T_IF: return "if";
        case T_ELSE: return "else";
        case T_RETURN: return "return";
        case T_ASSIGN: return "assignment operator '='";
        case T_PLUS: return "'+'";
        case T_MINUS: return "'-'";
        case T_MUL: return "'*'";
        case T_DIV: return "'/'";
        case T_LPAREN: return "'('";
        case T_RPAREN: return "')'";
        case T_LBRACE: return "'{'";
        case T_RBRACE: return "'}'";
        case T_SEMICOLON: return "semicolon ';'";
        case T_GT: return "'>'";
        case T_EOF: return "end of file";
        default: return "unknown token";
    }
}

    void parseBlock() {
        expect(T_LBRACE);  
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);  
    }
    void parseDeclaration() {
    TokenType varType;
    if (tokens[pos].type == T_INT) {
        varType = T_INT;
        pos++;
    } else if (tokens[pos].type == T_FLOAT) { // Support for float
        varType = T_FLOAT;
        pos++;
    } else {
        cout << "Syntax Error: Expected 'int' or 'float' but found '" 
             << tokens[pos].value << "' on line " << tokens[pos].line << endl;
        exit(1);
    }

    if (tokens[pos].type != T_ID) {
        cout << "Syntax Error: Expected an identifier after type declaration on line " 
             << tokens[pos].line << endl;
        exit(1);
    }

    string varName = tokens[pos].value;

    // Check if variable is already declared
    if (symbolTable.find(varName) != symbolTable.end()) {
        cout << "Error: Variable '" << varName << "' is already declared on line " 
             << tokens[pos].line << endl;
        exit(1);
    }

    symbolTable[varName] = varType; // Add to the symbol table
    pos++;

    // Optional initialization
    if (tokens[pos].type == T_ASSIGN) {
        pos++;
        TokenType exprType = parseExpression();
        if (exprType != varType) {
            cout << "Type Error: Cannot assign a value of type " 
                 << tokenTypeToString(exprType) << " to variable '" << varName 
                 << "' of type " << tokenTypeToString(varType) 
                 << " on line " << tokens[pos].line << endl;
            exit(1);
        }
    }

    expect(T_SEMICOLON); // Expect semicolon at the end of declaration
}


    void parseAssignment() {
    if (tokens[pos].type != T_ID) {
        cout << "Syntax Error: Expected an identifier for assignment on line " 
             << tokens[pos].line << endl;
        exit(1);
    }

    std::string varName = tokens[pos].value;

    // Check if the variable is declared
    if (symbolTable.find(varName) == symbolTable.end()) {
        cout << "Error: Variable '" << varName << "' is not declared on line " 
             << tokens[pos].line << endl;
        exit(1);
    }

    TokenType varType = symbolTable[varName]; // Get the type of the variable
    pos++; // Consume the identifier
    expect(T_ASSIGN); // Expect an '='

    TokenType exprType = parseExpression(); // Parse the expression and get its type

    // Check if the types match
    if (varType != exprType) {
        cout << "Type Error: Cannot assign a value of type " 
             << tokenTypeToString(exprType) << " to variable '" << varName 
             << "' of type " << tokenTypeToString(varType) 
             << " on line " << tokens[pos].line << endl;
        exit(1);
    }

    expect(T_SEMICOLON); // Assignment must end with a semicolon
}

    void parseIfStatement() {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();  
        if (tokens[pos].type == T_ELSE) {
            expect(T_ELSE);
            parseStatement();  
        }
    }

    void parseReturnStatement() {
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

TokenType parseFactor() {
    if (tokens[pos].type == T_NUM) {
        // Determine if the number is an integer or float
        if (tokens[pos].value.find('.') != string::npos) {
            pos++;
            return T_FLOAT; // Float constant
        } else {
            pos++;
            return T_INT; // Integer constant
        }
    } else if (tokens[pos].type == T_ID) {
        std::string varName = tokens[pos].value;

        // Check if the variable is declared
        if (symbolTable.find(varName) == symbolTable.end()) {
            cout << "Error: Variable '" << varName << "' is not declared on line " 
                 << tokens[pos].line << endl;
            exit(1);
        }

        TokenType varType = symbolTable[varName]; // Retrieve variable type
        pos++;
        return varType; // Return the variable's type
    } else if (tokens[pos].type == T_LPAREN) {
        expect(T_LPAREN);
        TokenType exprType = parseExpression(); // Parse the inner expression
        expect(T_RPAREN);
        return exprType; // Return the type of the expression
    } else {
        cout << "Syntax Error: Unexpected token '" << tokens[pos].value 
             << "' on line " << tokens[pos].line << endl;
        exit(1);
    }
}
TokenType parseTerm() {
    TokenType leftType = parseFactor(); // Parse the first factor and get its type

    while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
        pos++; // Consume operator

        TokenType rightType = parseFactor(); // Parse the next factor

        // Type-check: both sides of the operator must be compatible
        if (leftType != rightType) {
            cout << "Type Error: Mismatched types in term on line " 
                 << tokens[pos].line << endl;
            exit(1);
        }
    }

    return leftType; // Return the type of the resulting term
}
    TokenType parseExpression() {
    TokenType leftType = parseTerm(); // Parse the first term and get its type

    while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
        TokenType operatorToken = tokens[pos].type;
        pos++; // Consume operator

        TokenType rightType = parseTerm(); // Parse the next term

        // Type-check: both sides of the operator must be compatible
        if (leftType != rightType) {
            cout << "Type Error: Mismatched types in expression on line " 
                 << tokens[pos].line << endl;
            exit(1);
        }
    }

    return leftType; // Return the type of the resulting expression
}

    void expect(TokenType type) {
    if (tokens[pos].type == type) {
        pos++;
    } else {
        cout << "Syntax error on line " << tokens[pos].line << ": expected " 
             << tokenTypeToString(type) << ", but found '" << tokens[pos].value 
             << "' (" << tokenTypeToString(tokens[pos].type) << ")" << endl;
        exit(1);
    }
}

};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename.txt>" << endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file '" << argv[1] << "'. Please check the file path and permissions." << endl;
        return 1;
    }

    string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    try {
        parser.parseProgram();
    } catch (const exception& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1;
    }

    return 0;
}