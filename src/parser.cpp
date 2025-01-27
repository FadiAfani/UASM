#include "../include/parser.h"

namespace UASM {

    Parser::Parser(const std::vector<std::unique_ptr<UASM::Token>>& _tokens) : tokens(_tokens) {}

    void Parser::parse() {
        Token* token;
        while(cur_token < tokens.size()) {
            std::unique_ptr<Label> label = parse_label();
            labels.push_back(std::move(label));
        }
    }

    std::unique_ptr<Label> Parser::parse_label() {
        Token* t;
        Token* label = consume_token(IDENTIFIER_TOKEN, "expected a label");
        Token* colon = consume_token(COLON_TOKEN, "expected a ':' symbol");
        if (label == nullptr || colon == nullptr)
            return nullptr;
        auto node = std::make_unique<Label>();
        node->name = label;

        return node;

    }

    Token* Parser::consume_token(TokenType type, const char* err_msg) {
        Token* t = get_next_token();
        if (t == nullptr) {
            Token* last = tokens.back().get();
            logger.log(last->col, last->row, err_msg);
            return nullptr;
        }
        if (t->type != type)
            logger.log(t->col, t->row, err_msg);

        this->cur_token++;
        return t;
    }
}

