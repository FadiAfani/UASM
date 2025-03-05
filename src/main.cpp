#include "../include/tokenizer.h"
#include "../include/parser.h"
#include "../include/semantics.h"
#include "../include/cfg.h"
#include "../include/optimizer.h"
#include "../include/ssa_optimizer.h"
#include "../include/register_allocator.h"

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "specify a filepath argument" << std::endl;
    }
    UASM::Tokenizer tokenizer;
    UASM::Parser parser(tokenizer.get_tokens());
    tokenizer.tokenize(argv[1]);
    std::unique_ptr<UASM::Program> program = parser.parse();
    //UASM::Analyzer analyzer(std::move(program));
    //analyzer.analyze();
    UASM::Program* p = program.get();
    UASM::CFGBuilder cfg_builder(program.get());
    auto cfgs = cfg_builder.build();
    UASM::SSAOptimizer ssa_opt(cfgs.get());
    UASM::RegisterAllocator reg_alloc(cfgs.get());
    ssa_opt.optimize();
    reg_alloc.optimize();
    auto& intervals = reg_alloc.get_intervals();
    for (auto&[_, i] : intervals) {
        printf("(%d, %d)", i.first, i.second);
    }
    for (auto& [_, bbs] : cfgs->cfgs) {
        for (auto& bb : bbs)
            bb->print();
    }
    return 0;
}
