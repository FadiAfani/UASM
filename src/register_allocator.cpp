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
            liveness[ctx].in.insert(literal.symbol);
    }

    void RegisterAllocator::visit_binary_expr(BinaryExpr& expr) {
        expr.left.accept(*this);
        expr.right.accept(*this);
    }

    void RegisterAllocator::visit_assignment(Assignment& assignment) {
        std::string& id = assignment.identifier.variable.symbol;
        LivenessInfo& ctx_info = liveness[ctx];
        ctx_info.in.erase(id);

        std::visit([this](auto && e) { e.accept(*this); }, assignment.expr);

    }

    void RegisterAllocator::visit_jmp(JmpInst& inst) {
        inst.target.accept(*this);
        if (inst.cond.has_value())
            inst.cond->accept(*this);

    }

    void RegisterAllocator::visit_ret(Return& ret) {
        ret.value.accept(*this);
    }

    void RegisterAllocator::visit_label(Label& l) {}
    void RegisterAllocator::visit_func(Function& f) {}
    

    const std::unordered_map<std::string, std::pair<unsigned int, unsigned int>>& RegisterAllocator::get_intervals() {
        return intervals;
    }

    void RegisterAllocator::optimize() {
        for (auto&[_, bbs] : cd->cfgs) {
            for (auto& bb : bbs) {
                liveness[bb.get()] = LivenessInfo();
            }
            if (bbs.size() > 0) {
                for ( BasicBlock::PostOrder it(bbs.front().get()); !it.empty(); ++it) {
                    // compute out-state
                    ctx = &(*it);
                    ctx->print();
                    LivenessInfo& info = liveness[ctx];
                    for (BasicBlock* succ : it->successors) {
                        info.out.merge(liveness[succ].in);
                    }
                    // compute in-state
                    ctx->accept(*this);
                    for (auto it2 = info.out.begin(); it2 != info.out.end(); ++it2) {
                        info.in.erase(*it2);
                    }
                }
            }
        }
    }
}
