#ifndef CFG_H
#define CFG_H

#include "parser.h"
#include "program_visitor.h"

namespace UASM {

    struct BasicBlock {
        size_t tag;
        std::list<Instruction> instructions;
        std::list<BasicBlock*> successors;

        void print();
    };

    class ControlFlowGraph {
        private:
            std::vector<std::list<BasicBlock>> adj_list;
        public:
            void connect(size_t out, size_t in);

    };

    class CFGBuilder {
        private:
            std::unique_ptr<Program> program;
            std::unordered_map<Function*, std::list<std::unique_ptr<BasicBlock>>> cfgs;

        std::unordered_map<std::string, BasicBlock*> entries;
        public:
            CFGBuilder(std::unique_ptr<Program> _program);
            void build();
            void break_block(Function& func, Label& label);
            void compute_successors(BasicBlock* bb);
            std::unordered_map<Function*, std::list<std::unique_ptr<BasicBlock>>>& get_cfgs();
            


    };

}

#endif
