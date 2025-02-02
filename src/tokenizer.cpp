#include "../include/tokenizer.h"
#include <cstdlib>
#include <iostream>


UASM::Tokenizer::Tokenizer() {}

const std::vector<std::unique_ptr<UASM::Error>>& UASM::Tokenizer::get_errors() { return errors; }
std::vector<UASM::Token>& UASM::Tokenizer::get_tokens() { return tokens; }

void UASM::Tokenizer::handle_simple_token(std::string symbol, TokenType type) {
    Token token;
    token.col = this->col;
    this->col += symbol.size();
    token.row = this->row;
    token.type = type;
    token.symbol = symbol;
    this->tokens.push_back(std::move(token));
}

void UASM::Tokenizer::tokenize(const char* filename) {
    std::ifstream file(filename);
    char c;
    if (!file) {
        std::cout << "failed to open file" << std::endl;
    }

    while((c = file.get()) != -1) {
        switch(c) {
            case ':':
                handle_simple_token(":", COLON_TOKEN);
                break;
            
            case ',':
                handle_simple_token(":", COMMA_TOKEN);
                break;

            case '+':
                handle_simple_token("+", PLUS_TOKEN);
                break;
            case '-':
                handle_simple_token("-", MINUS_TOKEN);
                break;

            case '*':
                handle_simple_token("*", MUL_TOKEN);
                break;

            case '/':
                handle_simple_token("/", DIV_TOKEN);
                break;

            case '%':
                handle_simple_token("%", MOD_TOKEN);
                break;

            case '>':

                if (file.peek() == '=')
                    handle_simple_token(">=", BTE_TOKEN);
                else if (file.peek() == '>')
                    handle_simple_token(">>", RSHIFT_TOKEN);
                else
                    handle_simple_token(">", BT_TOKEN);
                break;

            case '<':
                if (file.peek() == '=')
                    handle_simple_token("<=", LTE_TOKEN);
                else if (file.peek() == '<')
                    handle_simple_token("<<", LSHIFT_TOKEN);
                else
                    handle_simple_token("<", LT_TOKEN);
                break;

            case '=':
                if (file.peek() == '=')
                    handle_simple_token("==", DEQ_TOKEN);
                else
                    handle_simple_token("=", EQ_TOKEN);
                break;

            case '(':
                handle_simple_token("(", LPAREN_TOKEN);
                break;

            case ')':
                handle_simple_token(")", RPAREN_TOKEN);
                break;

            case '{':
                handle_simple_token("{", LCURLY_TOKEN);
                break;

            case '}':
                handle_simple_token("}", RCURLY_TOKEN);
                break;

            case '@':
                handle_simple_token("@", AT_TOKEN);
                break;

            case '\n':
                this->col = 0;
                break;
            case '\t':
                this->col += 4;
                break;
            case '\r':
            case ' ':
                this->row++;
                break;

            default:
                /* identifier */
                Token token;
                
                if (std::isalpha(c) || c == '_') {
                    token.col = this->col;
                    token.row = this->row;
                    token.symbol += c;
                    c = file.peek();
                    while (std::isalpha(c) || std::isdigit(c) || c == '_') {
                        token.symbol += c;
                        file.get();
                        c = file.peek();
                        this->col++;
                    }

                    if (reserved_words.count(token.symbol) > 0)
                        token.type = reserved_words.at(token.symbol);
                    else
                        token.type = IDENTIFIER_TOKEN;

                /* number: float/integer */
                } else if (std::isdigit(c)) {
                    token.col = this->col;
                    token.row = this->row;
                    token.symbol += c;
                    while ((c = file.get()) && std::isdigit(c)) {
                        token.symbol += c;
                        this->col++;
                    }
                    token.type = INTEGER_TOKEN;
                    if (file.peek() == '.') {
                        token.type = FLOAT_TOKEN;
                        token.symbol += file.get();
                        while ((c = file.get()) && std::isdigit(c)) {
                            token.symbol += c;
                            this->col++;
                        }
                    }

                } else {
                    std::cout << "Unrecognized Token" << std::endl;
                    return;

                }

                this->tokens.push_back(std::move(token));
                break;

        }
    }

}

