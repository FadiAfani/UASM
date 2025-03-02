#include "../include/cfg.h"
#include <variant>

namespace UASM {

    void BasicBlock::print() {
        printf("\n-----------\n");
        printf("BasicBlock: %ld\n", tag);
        printf("Successors: ");
        for (BasicBlock* bb : successors) {
            printf("%ld-> ", bb->tag);
        }
        printf("\n");
        printf("Instructions: \n");
        for (Instruction& inst : instructions) {
            if (std::holds_alternative<JmpInst>(inst))
                std::get<JmpInst>(inst).print();

            if (std::holds_alternative<Return>(inst))
                std::get<Return>(inst).print();

            if (std::holds_alternative<Assignment>(inst))
                std::get<Assignment>(inst).print();

        }
        printf("\n-----------\n");
    }


    void CFGBuilder::compute_successors(BasicBlock* bb) {
        Instruction& last = bb->instructions.back();
        if (std::holds_alternative<JmpInst>(last)) {
            JmpInst& jmp = std::get<JmpInst>(last);
            bb->successors.push_back(cfg_data->entries[jmp.target.symbol]);
        }
    }

    CFGBuilder::CFGBuilder(Program* _program) : program(_program) {
        cfg_data = std::make_unique<CFGData>();
        for (auto&[_, func] : program->functions) {
            std::list<std::unique_ptr<BasicBlock>> l;
            for (auto&[_, label] : func.labels) {
                std::unique_ptr<BasicBlock> bb = std::make_unique<BasicBlock>();
                cfg_data->entries[label.name.symbol] = bb.get();
                bb->tag = l.size();
                l.push_back(std::move(bb));
            }

            cfg_data->cfgs[&func] = std::move(l);
        }
    }
    std::unique_ptr<CFGData> CFGBuilder::build() {
        for (auto&[_, func] : program->functions) {
            for (auto&[_, label] : func.labels) {
                break_block(func, label);
            }
            for (auto& bb : cfg_data->cfgs[&func]) {
                compute_successors(bb.get());
            }
        }
        return std::move(cfg_data);
    }

    void CFGBuilder::break_block(Function& func, Label& label) {
        auto& blocks = cfg_data->cfgs[&func];


        size_t i;
        BasicBlock* entry = cfg_data->entries[label.name.symbol];
        for (i = 0; i < label.instructions.size(); i++) {
            Instruction& inst = label.instructions[i];
            entry->instructions.push_back(inst);
            if (std::holds_alternative<JmpInst>(inst)
                || std::holds_alternative<Return>(inst))
                break;
        }

        BasicBlock* parent = entry;
        auto cur = std::make_unique<BasicBlock>();
        for (size_t j = i + 1; j < label.instructions.size(); j++) {
            Instruction& inst = label.instructions[j];
            cur->instructions.push_back(inst);
            if (std::holds_alternative<JmpInst>(inst)
                || std::holds_alternative<Return>(inst))
            {
                parent->successors.push_back(cur.get());
                cur->tag = blocks.size();
                parent = cur.get();
                blocks.push_back(std::move(cur));
                cur = std::make_unique<BasicBlock>();

            }

        }
    }

}
