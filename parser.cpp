#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>

using namespace std;

enum TokenType {
    T_INT, T_FLOAT, T_DOUBLE, T_STRING, T_BOOL, T_CHAR, T_ID, T_NUM, T_IF, T_ELSE, T_RETURN, 
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
                    else if (word == "double") tokens.push_back(Token{T_DOUBLE, word, line});
                    else if (word == "string") tokens.push_back(Token{T_STRING, word, line});
                    else if (word == "bool") tokens.push_back(Token{T_BOOL, word, line});
                    else if (word == "char") tokens.push_back(Token{T_CHAR, word, line});
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
            bool isFloat = false;
            while (pos < src.size() && (isdigit(src[pos]) || src[pos] == '.')) {
                if (src[pos] == '.') isFloat = true;
                pos++;
            }
            if (isFloat)
                return src.substr(start, pos - start);  // Will return float values as strings
            return src.substr(start, pos - start);      // Will return integer numbers as strings
        }

        string consumeWord() {
            size_t start = pos;
            while (pos < src.size() && isalnum(src[pos])) pos++;
            return src.substr(start, pos - start);
        }
};


class Parser {
 

public:
    Parser(const vector<Token> &tokens) {
        this->tokens = tokens;  
        this->pos = 0;          
    }

    void parseProgram() {
        while (tokens[pos].type != T_EOF) {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;
    map<string, TokenType> symbolTable;

    void parseStatement() {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_DOUBLE || 
            tokens[pos].type == T_STRING || tokens[pos].type == T_BOOL || tokens[pos].type == T_CHAR) {
            parseDeclaration();
        }else if (tokens[pos].type == T_ID) {
            parseAssignment();
        } else if (tokens[pos].type == T_IF) {
            parseIfStatement();
        } else if (tokens[pos].type == T_RETURN) {
            parseReturnStatement();
        } else if (tokens[pos].type == T_LBRACE) {  
            parseBlock();
        } else {
            cout << "Syntax error: unexpected token " << tokens[pos].value << endl;
            exit(1);
        }
    }
    string tokenTypeToString(TokenType type) {
    switch (type) {
        case T_ID: return "identifier";
        case T_INT: return "int";
        case T_FLOAT: return "float";
        case T_DOUBLE: return "double";
        case T_STRING: return "string";
        case T_BOOL: return "bool";
        case T_CHAR: return "char";
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
        TokenType declaredType = tokens[pos].type;  // Save the type of variable
        expect(tokens[pos].type);  // Expect the data type (int, float, etc.)
        string varName = tokens[pos].value;
        expect(T_ID);  // Expect an identifier (variable name)
        
        if (symbolTable.find(varName) != symbolTable.end()) {
            cout << "Error: Variable '" << varName << "' already declared on line " << tokens[pos].line << endl;
            exit(1);
        }

        symbolTable[varName] = declaredType;  // Add the variable and its type to the symbol table
        expect(T_SEMICOLON);
    }

     void parseAssignment() {
        string varName = tokens[pos].value;
        expect(T_ID);

        if (symbolTable.find(varName) == symbolTable.end()) {
            cout << "Error: Variable '" << varName << "' not declared before assignment on line " << tokens[pos].line << endl;
            exit(1);
        }

        TokenType expectedType = symbolTable[varName];  // Get the declared type of the variable
        expect(T_ASSIGN);
        TokenType exprType = parseExpression();  // Get the type of the assigned expression

        // Check for type mismatch
        if (expectedType == T_INT && exprType == T_FLOAT) {
            cout << "Error: Cannot assign float value to an int variable '" << varName << "' on line " << tokens[pos].line << endl;
            exit(1);
        }

        expect(T_SEMICOLON);
    }
    TokenType parseExpression() {
        TokenType termType = parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
            pos++;
            parseTerm();  // Here, we assume the expression returns the correct type (for simplicity)
        }
        return termType;
    }
    TokenType parseTerm() {
        TokenType factorType = parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
            pos++;
            parseFactor();  // We assume the term returns the correct type (for simplicity)
        }
        return factorType;
    }

    TokenType parseFactor() {
        if (tokens[pos].type == T_NUM) {
            string value = tokens[pos].value;
            pos++;
            return (value.find('.') != string::npos) ? T_FLOAT : T_INT;  // Return float if it contains a dot
        } else if (tokens[pos].type == T_ID) {
            string varName = tokens[pos].value;
            if (symbolTable.find(varName) == symbolTable.end()) {
                cout << "Error: Variable '" << varName << "' not declared on line " << tokens[pos].line << endl;
                exit(1);
            }
            TokenType varType = symbolTable[varName];
            pos++;
            return varType;
        } else if (tokens[pos].type == T_LPAREN) {
            expect(T_LPAREN);
            TokenType exprType = parseExpression();
            expect(T_RPAREN);
            return exprType;
        } else {
            cout << "Syntax error: unexpected token " << tokens[pos].value << endl;
            exit(1);
        }
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

    void parseBlock() {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void expect(TokenType type) {
        if (tokens[pos].type == type) {
            pos++;
        } else {
            cout << "Syntax error: expected " << tokenTypeToString(type)
                 << " but found '" << tokens[pos].value
                 << "' on line " << tokens[pos].line << endl;
            exit(1);
        }
    }

};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << argv[0] << "<filename.txt>" <<endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        cerr << "Could not open the file: " << argv[1] << endl;
        return 1;
    }

    string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    Lexer lexer(input);
    vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}