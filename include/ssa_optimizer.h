#ifndef SSA_OPTIMIZER
#define SSA_OPTIMIZER

#include "optimizer.h"

struct AltSymbol {
    unsigned int dups;
    std::string& new_symbol;

    AltSymbol(std::string& _new_symbol) : new_symbol(_new_symbol) {}
};

namespace UASM {
    class SSAOptimizer : public CFGOptimizer {
        private:
            unsigned int temps = 0;
            std::unordered_map<std::string, AltSymbol> dups;
        public:
            SSAOptimizer(CFGData* _cd);
            void visit_binary_expr(BinaryExpr& expr) override;
            void visit_literal(Token& literal) override;
            void visit_assignment(Assignment& inst) override;
            void visit_jmp(JmpInst& inst) override;
            void visit_ret(Return& ret) override;
            void visit_label(Label& label) override;
            void visit_func(Function& func) override;

    };
}



#endif
