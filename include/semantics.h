#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "../include/parser.h"

#include <string>

namespace UASM {

    class Analyzer {
        private:
            std::unique_ptr<Program> program = nullptr;
        public:
            Analyzer(std::unique_ptr<Program> program);
            void analyze();
            void verify_expr(Function& func, Expr& expr);
            void verify_binary_expr(Function& func, BinaryExpr& expr, TokenType expected_type);
            void verify_literal(Function& func, Token* literal, TokenType expected_type);
            void verify_assignment(Function& func, Assignment& inst);
            void verify_jmp(Function& func, JmpInst& inst);
            void verify_ret(Function& func, Token* ret_val);
            void verify_label(Function& func, Label& label);
            void verify_func(Function& func);
            TokenType get_operand_type(Function& func, Token* operand);
    };
}

#endif
