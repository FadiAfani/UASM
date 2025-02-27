#ifndef PROGRAM_VISITOR
#define PROGRAM_VISITOR

#include "parser.h"

namespace UASM {

    class ProgramVisitor {
        public:
            virtual ~ProgramVisitor() {}
            virtual void visit_binary_expr(BinaryExpr& expr) = 0;
            virtual void visit_literal(Token* literal) = 0;
            virtual void visit_assignment(Assignment& inst) = 0;
            virtual void visit_jmp(JmpInst& inst) = 0;
            virtual void visit_ret(Return& ret_val) = 0;
            virtual void visit_label(Label& label) = 0;
            virtual void visit_func(Function& func) = 0;

    };

}


#endif
