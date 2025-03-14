#ifndef REGISTER_ALLOCATOR_H
#define REGISTER_ALLOCATOR_H

#include "optimizer.h"

using Interval = std::pair<unsigned int, unsigned int>;

namespace UASM {

    class RegisterAllocator : public CFGOptimizer {
        private:
            std::unordered_map<std::string, std::vector<std::reference_wrapper<std::string>>> graph;
            std::unordered_map<std::string, Interval> intervals;
            unsigned int cur_inst = 0;
        public:
            RegisterAllocator(CFGData* _cd);
            void optimize() override;
            void visit_instruction(Instruction& inst) override;
            void connect(std::string& in, std::string& out);
            void visit_binary_expr(BinaryExpr& expr) override;
            void visit_literal(Token& literal) override;
            void visit_assignment(Assignment& inst) override;
            void visit_jmp(JmpInst& inst) override;
            void visit_ret(Return& ret) override;
            void visit_label(Label& label) override;
            void visit_func(Function& func) override;
            const std::unordered_map<std::string, std::pair<unsigned int, unsigned int>>& get_intervals();


    };

}


#endif 
