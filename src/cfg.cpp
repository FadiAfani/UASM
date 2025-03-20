#include "../include/cfg.h"
#include <variant>

namespace UASM {

    void CFGBuilder::compute_successors(BasicBlock* bb) {
        Instruction& last = bb->instructions.back();
        if (std::holds_alternative<JmpInst>(last)) {
            JmpInst& jmp = std::get<JmpInst>(last);
            BasicBlock* target = cfg_data->entries[jmp.target.symbol];
            target->predecessors.push_back(bb);
            bb->successors.push_back(target);
        }
    }

    CFGBuilder::CFGBuilder(Program* _program) : program(_program) {
        cfg_data = std::make_unique<CFGData>();
        for (auto&[_, func] : program->functions) {
            std::list<std::unique_ptr<BasicBlock>> l;
            auto items = std::vector<std::pair<std::string, Label>>(func.labels.begin(), func.labels.end());
            std::sort(items.begin(), items.end(), [](std::pair<std::string, Label>& a, std::pair<std::string, Label>& b) {
                    return a.second.order < b.second.order; });
            for (auto&[_, label] : items) {
                std::unique_ptr<BasicBlock> bb = std::make_unique<BasicBlock>(func);
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
        auto cur = std::make_unique<BasicBlock>(func);
        for (size_t j = i + 1; j < label.instructions.size(); j++) {
            Instruction& inst = label.instructions[j];
            cur->instructions.push_back(inst);
            if (std::holds_alternative<JmpInst>(inst)
                || std::holds_alternative<Return>(inst))
            {
                parent->successors.push_back(cur.get());
                cur->tag = blocks.size();
                cur->predecessors.push_back(parent);
                parent = cur.get();
                blocks.push_back(std::move(cur));
                cur = std::make_unique<BasicBlock>(func);

            }

        }
    }

}
