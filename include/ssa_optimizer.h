#ifndef SSA_OPTIMIZER
#define SSA_OPTIMIZER

#include "optimizer.h"

namespace UASM {
    class SSAOptimizer : Optimizer {
        private:
            unsigned int temps = 0;
            std::pair<Function*, Label*> ctx;
        public:
            SSAOptimizer(std::unique_ptr<Program> _program);
            void optimize() override;
            void visit_binary_expr(BinaryExpr& expr) override;
            void visit_literal(Token* literal) override;
            void visit_assignment(Assignment& inst) override;
            void visit_jmp(JmpInst& inst) override;
            void visit_ret(Return& ret_val) override;
            void visit_label(Label& label) override;
            void visit_func(Function& func) override;
            void assign_temp(Token* literal);

    };

}



#endif
