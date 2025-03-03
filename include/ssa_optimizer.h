#ifndef SSA_OPTIMIZER
#define SSA_OPTIMIZER

#include "optimizer.h"

namespace UASM {
    class SSAOptimizer : CFGOptimizer {
        private:
            unsigned int temps = 0;
            BasicBlock* ctx;
            std::unordered_map<Symbol*, unsigned int> dups;
        public:
            void visit_binary_expr(BinaryExpr& expr) override;
            void visit_literal(Token& literal) override;
            void visit_assignment(Assignment& inst) override;
            void visit_jmp(JmpInst& inst) override;
            void visit_ret(Return& ret) override;
            void visit_label(Label& label) override;
            void visit_func(Function& func) override;
            void assign_temp(Token* literal);

    };
}



#endif
