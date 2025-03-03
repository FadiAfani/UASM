#ifndef CFG_H
#define CFG_H

#include "visitor.h"

namespace UASM {


    using FunctionGraphMap = std::unordered_map<Function*, std::list<std::unique_ptr<BasicBlock>>>;

    struct CFGData {
        FunctionGraphMap cfgs;
        std::unordered_map<std::string, BasicBlock*> entries;
    };

    class CFGBuilder {
        private:
            Program* program;
            std::unique_ptr<CFGData> cfg_data;
        public:
            CFGBuilder(Program* _program);
            std::unique_ptr<CFGData> build();
            void break_block(Function& func, Label& label);
            void compute_successors(BasicBlock* bb);
            


    };

}

#endif
