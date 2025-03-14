#include "../include/register_allocator.h"

namespace UASM {

    RegisterAllocator::RegisterAllocator(CFGData* _cd) : CFGOptimizer(_cd) {}

    void RegisterAllocator::connect(std::string& in, std::string& out) {
        graph[in].push_back(out);
        graph[out].push_back(in);
    }

    void RegisterAllocator::visit_instruction(Instruction& inst) {

        std::visit([this](auto&& i) {
                i.accept(*this);
        }, inst);

        cur_inst++;
    }

    void RegisterAllocator::visit_literal(Token& literal) {
        if (literal.type == IDENTIFIER_TOKEN)
            intervals[literal.symbol].second = cur_inst;
    }

    void RegisterAllocator::visit_binary_expr(BinaryExpr& expr) {
    }

    void RegisterAllocator::visit_assignment(Assignment& assignment) {
        intervals[assignment.identifier.variable.symbol].first = cur_inst;
        intervals[assignment.identifier.variable.symbol].second = cur_inst;
    }

    void RegisterAllocator::visit_jmp(JmpInst& inst) {

    }

    void RegisterAllocator::visit_ret(Return& ret) {
    }

    void RegisterAllocator::visit_label(Label& l) {}
    void RegisterAllocator::visit_func(Function& f) {}
    

    const std::unordered_map<std::string, std::pair<unsigned int, unsigned int>>& RegisterAllocator::get_intervals() {
        return intervals;
    }

    void RegisterAllocator::optimize() {
        for (auto&[_, bbs] : cd->cfgs) {
            if (bbs.size() > 0) {
                for ( BasicBlock::PostOrder it(bbs.front().get()); !it.empty(); ++it) {
                    it->accept(*this);
                }
            }
        }
    }
}
