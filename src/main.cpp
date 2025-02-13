#include "../include/tokenizer.h"
#include "../include/parser.h"
#include "../include/semantics.h"

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "specify a filepath argument" << std::endl;
    }
    UASM::Tokenizer tokenizer;
    UASM::Parser parser(tokenizer.get_tokens());
    tokenizer.tokenize(argv[1]);
    parser.parse();
    UASM::Analyzer analyzer(parser.get_functions());
    analyzer.analyze();
    std::cout << parser.get_functions().size() << std::endl;
    for (auto& err : parser.get_errors())
        err->print();
    return 0;
}
