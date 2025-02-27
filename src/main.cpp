#include "../include/tokenizer.h"
#include "../include/parser.h"
#include "../include/semantics.h"
#include "../include/cfg.h"

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
    UASM::CFGBuilder cfg_builder(std::move(program));
    cfg_builder.build();
    for (auto& [_, bbs] : cfg_builder.get_cfgs()) {
        for (auto& bb : bbs)
            bb->print();
    }
    return 0;
}
