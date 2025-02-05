#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "../include/parser.h"

#include <string>

namespace UASM {

    class Analyzer {
        private:
            const std::unordered_map<std::string, Function>& functions;
            std::unordered_map<std::string, Symbol>& symbols;
        public:
            Analyzer(const std::unordered_map<std::string, Function>& _functions, std::unordered_map<std::string, Symbol>& _symbols);
            void verify_expr(Function& func, Expr& expr);
            void verify_binary_expr(Function& func, BinaryExpr& expr, TokenType expected_type);
            void verify_literal(Function& func, Token* literal, TokenType expected_type);
            void verify_assignment(Function& func, Assignment& inst);
            void verify_jmp(Function& func, JmpInst& inst);
            void verify_ret(Function& func, Token* ret_val);
            void verify_label(Function& func, Label& label);
            void verify_func(Function& func);
    };
}

#endif
