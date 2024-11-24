#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>

using namespace std;

enum TokenType {
    T_INT, T_ID, T_NUM,T_FLOAT, T_DOUBLE, T_BOOL, T_CHAR, T_STRING,
    T_IF, T_ELSE,T_ELSEIF, T_RETURN,
    T_BOOL_LITERAL,T_TRUE,T_FALSE, 
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, 
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,  
    T_SEMICOLON, T_EOF, 
    // Relational operator tokens
    T_EQ,      // ==
    T_NEQ,     // !=
    T_LT,      // <
    T_GT,      // >
    T_LTE,     // <=
    T_GTE,     // >=
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

                if (current == '\n') { // Handle new line
                    line++;
                    pos++;
                    continue;
                }
                if (current == '/' && pos + 1 < src.size() && src[pos + 1] == '/') { 
                    // Handle single-line comments (//)
                    while (pos < src.size() && src[pos] != '\n') {
                        pos++; // Skip until the end of the line
                    }
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
                    else if (word == "bool") tokens.push_back(Token{T_BOOL, word, line});
                    else if (word == "char") tokens.push_back(Token{T_CHAR, word, line});
                    else if (word == "string") tokens.push_back(Token{T_STRING, word, line});
                    else if (word == "if") tokens.push_back(Token{T_IF, word, line});
                    else if (word == "else") tokens.push_back(Token{T_ELSE, word, line});
                    else if (word == "elseif") tokens.push_back(Token{T_ELSEIF, word, line});
                    else if (word == "return") tokens.push_back(Token{T_RETURN, word, line});
                    else if (word == "true") tokens.push_back(Token{T_TRUE, word, line});
                    else if (word == "false") tokens.push_back(Token{T_FALSE, word, line});
                    else tokens.push_back(Token{T_ID, word, line});
                    continue;
                }

                if (current == '\'') { // Handle character literals
                    pos++;
                    if (pos < src.size() - 1 && src[pos + 1] == '\'') {
                        string charLiteral(1, src[pos]);
                        tokens.push_back(Token{T_NUM, charLiteral, line});
                        pos += 2; // Skip past the character and closing quote
                    } else {
                        cout << "Error: Malformed character literal on line " << line << endl;
                        exit(1);
                    }
                    continue;
                }

                if (current == '"') { // Handle string literals
                    pos++;
                    string strLiteral;
                    while (pos < src.size() && src[pos] != '"') {
                        strLiteral += src[pos];
                        pos++;
                    }
                    if (pos < src.size() && src[pos] == '"') {
                        tokens.push_back(Token{T_NUM, strLiteral, line}); // Assuming T_NUM for now
                        pos++; // Skip closing quote
                    } else {
                        cout << "Error: Malformed string literal on line " << line << endl;
                        exit(1);
                    }
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
private:
    vector<Token> tokens;
    size_t pos;
    std::map<std::string, TokenType> symbolTable;


 void expect(TokenType type) {
        if (tokens[pos].type == type) {
            pos++;
        } else {
            cout << "Syntax error on line " << tokens[pos].line << ": expected "
                 << tokenTypeToString(type) << " but found '" << tokens[pos].value << "'" << endl;
            exit(1);
        }
}
    string tokenTypeToString(TokenType type) {
    switch (type) {
        case T_ID: return "identifier";
        case T_INT: return "int";
        case T_FLOAT: return "float"; 
        case T_DOUBLE: return "double";
        case T_BOOL: return "bool";
        case T_TRUE: return "bool";// true
        case T_FALSE: return "bool";// false
        case T_CHAR: return "char";
        case T_STRING: return "string";
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
 void parseDeclaration() {
        TokenType varType = tokens[pos].type; // T_INT or T_FLOAT
        pos++;

        if (tokens[pos].type != T_ID) {
            cout << "Syntax error on line " << tokens[pos].line << ": expected variable name after type declaration" << endl;
            exit(1);
        }

        string varName = tokens[pos].value;

        if (symbolTable.find(varName) != symbolTable.end()) {
            cout << "Error: Variable '" << varName << "' is already declared on line " << tokens[pos].line << endl;
            exit(1);
        }

        symbolTable[varName] = varType; // Add variable to symbol table
        pos++;

        if (tokens[pos].type == T_ASSIGN) {
            pos++;
            parseExpression(); // Validate expression
        }

        expect(T_SEMICOLON); // Ensure declaration ends with a semicolon
    }
   void parseStatement() {
    if (tokens[pos].type == T_IF) {
        parseIfStatement();
    } 
    else if (tokens[pos].type == T_RETURN) {
        parseReturnStatement();
    }
    else {
        parseExpression();
        expect(T_SEMICOLON); // Ensure valid statements
        }
    }
     void parseReturnStatement() {
        expect(T_RETURN);  // Handle the 'return' keyword
        parseExpression();  // The expression after 'return'
        expect(T_SEMICOLON);  // Ensure it's followed by a semicolon
    }
    void parseIfStatement() {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();  // Condition inside the if
        expect(T_RPAREN);
        expect(T_LBRACE);
        parseBlock();  // Block for the 'if' statement
        expect(T_RBRACE);

        // Check for 'else' or 'elseif'
        if (tokens[pos].type == T_ELSE) {
            pos++;  // Move past the 'else'
            if (tokens[pos].type == T_IF) {
                parseIfStatement();  // Handle 'elseif' recursively
            } else {
                expect(T_LBRACE);
                parseBlock();  // Handle regular 'else' block
                expect(T_RBRACE);
            }
        }
    }

    void parseBlock() {
        // Assuming a block contains multiple statements, which can be other conditionals or expressions
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }
    }

    void parseExpression() {
    parseTerm();
    while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS) {
        pos++;
        parseTerm();
    }
    if (tokens[pos].type == T_GT) {
        pos++;
        parseExpression();  // After relational operator, parse the next expression
    }
    // Add handling for boolean literals true/false
    if (tokens[pos].type == T_TRUE || tokens[pos].type == T_FALSE) {
        pos++; // Move past the true/false literal
    }
}

void parseTerm() {
    parseFactor();
    while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV) {
        pos++;
        parseFactor();
    }
}

void parseFactor() {
    if (tokens[pos].type == T_NUM || tokens[pos].type == T_ID || tokens[pos].type == T_TRUE || tokens[pos].type == T_FALSE) {
        pos++; // Consume the token
    } 
    else if (tokens[pos].type == T_ID) {
        pos++;  // Consume the identifier (variable)
    }
    else if (tokens[pos].type == T_LPAREN) {
        pos++;
        //expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
    } else {
        cout << "Syntax error: unexpected token " << tokens[pos].value <<" " << tokens[pos].line << endl;
        exit(1);
    }
}

public:
    Parser(const vector<Token>& tokens) : tokens(tokens), pos(0) {}

    void parseProgram() {
          while (tokens[pos].type != T_EOF) {
            
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_DOUBLE ||
            tokens[pos].type == T_BOOL || tokens[pos].type == T_CHAR || tokens[pos].type == T_STRING) {
            parseDeclaration();
        } 
        else if (tokens[pos].type == T_IF || tokens[pos].type == T_ELSE || tokens[pos].type == T_ELSEIF) {
            parseConditionalBlock(); // Handle if, else if, and else blocks
        }
        else {
            //parseExpression();
            parseStatement();
            expect(T_SEMICOLON); // Ensure valid statements
        }
    }
     if (tokens[pos].type == T_EOF) {
        cout << "End of file reached. Parsing completed successfully! No syntax errors." << endl;
    } else {
        cerr << "Unexpected token at the end of the file: " << tokens[pos].value << endl;
        exit(1);
    }
    cout << "Parsing completed successfully! No syntax errors." << endl;
    }

//for bool literals true and false 

void parseConditionalBlock() {
    // Parse an 'if', 'else if', or 'else' block
    expect(T_IF); // Expect an 'if' keyword
    expect(T_LPAREN); // Expect '('
    parseExpression(); // Parse the condition
    expect(T_RPAREN); // Expect ')'
    expect(T_LBRACE); // Expect '{'

    // Parse the block content
    while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
        parseStatement(); // Parse individual statements within the block
    }

    expect(T_RBRACE); // Ensure the block ends with '}'

    // Handle 'else if' and 'else' blocks recursively
    if (tokens[pos].type == T_ELSEIF) {
        parseConditionalBlock(); // Handle 'else if'
    } else if (tokens[pos].type == T_ELSE) {
        expect(T_ELSE); // Expect 'else'
        expect(T_LBRACE); // Expect '{'

        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF) {
            parseStatement();
        }

        expect(T_RBRACE); // Ensure the 'else' block ends with '}'
    }
}
void parseCondition() {
    // Parse the left-hand side of the condition
    parseExpression();

    // Check for relational operators
    if (tokens[pos].type == T_EQ || tokens[pos].type == T_NEQ ||
        tokens[pos].type == T_LT || tokens[pos].type == T_GT ||
        tokens[pos].type == T_LTE || tokens[pos].type == T_GTE) {
        pos++; // Consume the operator
        parseExpression(); // Parse the right-hand side of the condition

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