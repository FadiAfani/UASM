#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <optional>
#include "../include/tokenizer.h"

namespace UASM {

    struct Operand {
        Token* variable = nullptr;
        Token* type = nullptr;
    };


    struct Instruction {
        Token* opcode = nullptr;
        Operand op1;
        Operand op2;
        Operand op3;
    };

    struct Label {
        Token* name;
        std::vector<Instruction> instructions;
    };

    class Parser {
        private:
            const std::vector<std::unique_ptr<Token>>& tokens;
            std::vector<std::unique_ptr<Error>> errors;
            std::vector<std::unique_ptr<Label>> labels;
            ErrorLogger logger;
            size_t cur_token = 0;
        
        public:
            Parser(const std::vector<std::unique_ptr<Token>>& _tokens);
            void parse();
            Token* consume_any(std::initializer_list<TokenType> types, const char* err_msg);
            Token* consume_token(TokenType type, const char* err_msg);
            Token* get_next_token();
            Token* peek();
            void parse_label();
            void parse_instruction(Label* label);
            void parse_operand(Instruction* inst);
            void parse_variable(Operand& operand);
            void get_parser_errors();
            void get_instructions();

    };

}


#endif
