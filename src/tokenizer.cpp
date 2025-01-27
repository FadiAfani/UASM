#include "../include/tokenizer.h"
#include <cstdlib>
#include <iostream>


UASM::Tokenizer::Tokenizer() {}

const std::vector<std::unique_ptr<UASM::Error>>& UASM::Tokenizer::get_errors() { return errors; }
const std::vector<std::unique_ptr<UASM::Token>>& UASM::Tokenizer::get_tokens() { return tokens; }

void UASM::Tokenizer::tokenize(const char* filename) {
    std::ifstream file(filename);
    char c;
    if (!file) {
        std::cout << "failed to open file" << std::endl;
    }

    while((c = file.get()) != -1) {
        std::unique_ptr<Token> token = std::make_unique<Token>();
        switch(c) {
            case ':':
                token->col = ++this->col;
                token->row = this->row;
                token->type = COLON_TOKEN;
                token->symbol = ":";
                break;
            
            case ',':
                token->col = ++this->col;
                token->row = this->row;
                token->type = COMMA_TOKEN;
                token->symbol = ",";
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
                if (std::isalpha(c) || c == '_') {
                    token->col = this->col;
                    token->row = this->row;
                    token->symbol += c;
                    c = file.peek();
                    while (std::isalpha(c) || std::isdigit(c) || c == '_') {
                        c = file.get();
                        token->symbol += c;
                        this->col++;
                    }
                /* number: float/integer */
                } else if (std::isdigit(c)) {
                    token->col = this->col;
                    token->row = this->row;
                    token->symbol += c;
                    while ((c = file.get()) && std::isdigit(c)) {
                        token->symbol += c;
                        this->col++;
                    }
                    token->type = INTEGER_TOKEN;
                    if (file.peek() == '.') {
                        token->type = FLOAT_TOKEN;
                        token->symbol += file.get();
                        while ((c = file.get()) && std::isdigit(c)) {
                            token->symbol += c;
                            this->col++;
                        }
                    }

                } else {
                    std::cout << "Unrecognized Token" << std::endl;
                    return;

                }

        }
        this->tokens.push_back(std::move(token));
    }

}

