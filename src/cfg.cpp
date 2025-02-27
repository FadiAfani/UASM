#include "../include/cfg.h"
#include <variant>

namespace UASM {

    void BasicBlock::print() {
        printf("\n-----------\n");
        printf("BasicBlock: %ld\n", tag);
        printf("Successors: ");
        for (BasicBlock* bb : successors) {
            printf("%ld -> ", bb->tag);
        }
        printf("\n-----------\n");
    }

    std::unordered_map<Function*, std::list<std::unique_ptr<BasicBlock>>>& CFGBuilder::get_cfgs() {
        return cfgs;
    }


    void CFGBuilder::compute_successors(BasicBlock* bb) {
        Instruction& last = bb->instructions.back();
        if (std::holds_alternative<JmpInst>(last)) {
            JmpInst& jmp = std::get<JmpInst>(last);
            bb->successors.push_back(entries[jmp.target->symbol]);
        }
    }

    CFGBuilder::CFGBuilder(std::unique_ptr<Program> _program) : program(std::move(_program)) {
        for (auto&[_, func] : program->functions) {
            for (auto&[_, label] : func.labels) {
                std::unique_ptr<BasicBlock> bb = std::make_unique<BasicBlock>();
                entries[label.name->symbol] = bb.get();
                bb->tag = cfgs[&func].size();
                cfgs[&func].push_back(std::move(bb));
            }
        }
    }

    void CFGBuilder::build() {
        for (auto&[_, func] : program->functions) {
            for (auto&[_, label] : func.labels) {
                break_block(func, label);
            }
            for (auto& bb : cfgs[&func]) {
                compute_successors(bb.get());
            }
        }

    }

    void CFGBuilder::break_block(Function& func, Label& label) {
        auto& blocks = cfgs[&func];

        size_t i;
        BasicBlock* entry = entries[label.name->symbol];
        for (i = 0; i < label.instructions.size(); i++) {
            Instruction& inst = label.instructions[i];
            entry->instructions.push_back(inst);
            if (std::holds_alternative<JmpInst>(inst)
                || std::holds_alternative<Return>(inst))
                break;
        }

        entry->tag = blocks.size();

        auto cur = std::make_unique<BasicBlock>();
        BasicBlock* raw_cur = cur.get();
        for (size_t j = i; j < label.instructions.size(); j++) {
            Instruction& inst = label.instructions[i];
            cur->instructions.push_back(inst);
            if (std::holds_alternative<JmpInst>(inst)
                || std::holds_alternative<Return>(inst))
            {
                entry->successors.push_back(cur.get());
                blocks.push_back(std::move(cur));
                raw_cur->tag = blocks.size();

            }

        }
    }

}
