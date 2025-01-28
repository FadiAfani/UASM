#include "../include/parser.h"

namespace UASM {

    Parser::Parser(const std::vector<std::unique_ptr<UASM::Token>>& _tokens) : tokens(_tokens) {}

    void Parser::parse() {
        Token* token;
        while(cur_token < tokens.size())
            parse_label();
        
    }

    void Parser::parse_label() {
        Token* t;
        Token* label = consume_token(IDENTIFIER_TOKEN, "expected a label");
        Token* colon = consume_token(COLON_TOKEN, "expected a ':' symbol");
        if (label == nullptr || colon == nullptr)
            return;
        auto node = std::make_unique<Label>();
        node->name = label;
        labels.push_back(std::move(node));

    }

    void Parser::parse_instruction(Label* label) {
        Token* t = get_next_token();
        Instruction inst;

        if (t == nullptr)
            return;

        switch(t->type) {
            case ADDI_OPCODE_TOKEN:
                parse_operand(&inst);
                parse_operand(&inst);
                parse_operand(&inst);
                break;
            default:
                logger.log(t->col, t->row, "invalid opcode");
        }

    }

    void Parser::parse_operand(Instruction* inst) {
        Operand op;
        Token* t;
        if ((t = peek(1)) != nullptr && t->type == COLON_TOKEN)
            parse_variable(op);
        else {

        t = consume_any(
                {
                IDENTIFIER_TOKEN, INTEGER_TOKEN, FLOAT_TOKEN
                }, 
                "an operand must eithr be a variable or a number"
                );
        if (t != nullptr && t->type == INTEGER_TOKEN || t->type == FLOAT_TOKEN)
            op.as_number = t;
        else
            op.as_variable.variable = t;
        }


    }

    void Parser::parse_variable(Operand& operand) {
        Token* t = consume_token(IDENTIFIER_TOKEN, "expected a register");
        if (t == nullptr)
            return;
        operand.as_variable.variable = t;
        t = consume_token(COLON_TOKEN, "expected a ':' symbol");
        if (t == nullptr)
            return;
        t = consume_any(
                {
                    I8_TYPE_TOKEN,
                    I16_TYPE_TOKEN,
                    I32_TYPE_TOKEN,
                    I64_TYPE_TOKEN,
                    F32_TYPE_TOKEN,
                    F64_TYPE_TOKEN,
                    U8_TYPE_TOKEN,
                    U16_TYPE_TOKEN,
                    U32_TYPE_TOKEN,
                    U64_TYPE_TOKEN
                }
                , 
                "missing type");
        if (t != nullptr)
            operand.as_variable.type = t;
        
    }

    Token* Parser::peek(size_t n) {
        if (cur_token + n > tokens.size())
            return nullptr;

        return tokens.at(cur_token + n).get();

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

