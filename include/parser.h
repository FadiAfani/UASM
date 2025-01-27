#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include "../include/tokenizer.h"

namespace UASM {

    struct Operand {
        Token* variable;
        Token* type;
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
            Token* consume_token(TokenType type, const char* err_msg);
            Token* get_next_token();
            std::unique_ptr<Label> parse_label();
            Instruction parse_instruction();
            Operand parse_operand();
            void get_parser_errors();
            void get_instructions();

    };

}


#endif
