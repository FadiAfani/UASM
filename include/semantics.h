#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "parser.h"
#include "program_visitor.h"

#include <string>

namespace UASM {

    class Analyzer : ProgramVisitor {
        private:
            std::unique_ptr<Program> program = nullptr;
            std::unordered_map<Unit*, TokenType> expr_types;
            Function* ctx;
        public:
            Analyzer(std::unique_ptr<Program> program);
            void analyze();
            void visit_binary_expr(BinaryExpr& expr) override;
            void visit_literal(Token* literal) override;
            void visit_assignment(Assignment& inst) override;
            void visit_jmp(JmpInst& inst) override;
            void visit_ret(Return& ret_val) override;
            void visit_label(Label& label) override;
            void visit_func(Function& func) override;
            TokenType get_operand_type(Token* operand);
    };
}

#endif
