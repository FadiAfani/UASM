#include "../include/cfg.h"
#include <variant>

namespace UASM {

    void CFGBuilder::compute_neighbors(std::list<std::unique_ptr<BasicBlock>>::iterator& it) {
        BasicBlock* bb = it->get();
        auto& blocks = cfg_data->cfgs[&bb->pf];
        Instruction& last = bb->instructions.back();
        if (std::holds_alternative<JmpInst>(last)) {
            JmpInst& jmp = std::get<JmpInst>(last);
            BasicBlock* target = cfg_data->entries[jmp.target.symbol];
            target->predecessors.push_back(bb);
            bb->successors.push_back(target);
            if (jmp.cond.has_value() && std::next(it) != blocks.end()) {
                BasicBlock* next = (*(std::next(it))).get();
                bb->successors.push_back(next);
                next->predecessors.push_back(bb);
            }
        }
    }

    CFGBuilder::CFGBuilder(Program* _program) : program(_program), cfg_data(std::make_unique<CFGData>()) {}

    std::unique_ptr<CFGData> CFGBuilder::build() {
        for (auto&[_, func] : program->functions) {
            cfg_data->cfgs[&func] = std::list<std::unique_ptr<BasicBlock>>();
            for (auto&[_, label] : func.labels) {
                break_block(func, label);
            }
            auto& blocks = cfg_data->cfgs[&func];
            size_t tags = 0;
            for (std::list<std::unique_ptr<BasicBlock>>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
                it->get()->tag = ++tags;
                compute_neighbors(it);
            }
        }
        return std::move(cfg_data);
    }

    void CFGBuilder::break_block(Function& func, Label& label) {
        auto& blocks = cfg_data->cfgs[&func];

        auto cur = std::make_unique<BasicBlock>(func);
        cfg_data->entries[label.name.symbol] = cur.get();

        for (Instruction& inst : label.instructions) {
            cur->instructions.push_back(inst);
            if (std::holds_alternative<JmpInst>(inst)
                    || std::holds_alternative<Return>(inst)) {
                blocks.push_back(std::move(cur));
                cur = std::make_unique<BasicBlock>(func);
            }
        }

    }

}
