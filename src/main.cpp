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
    std::unique_ptr<UASM::Program> program = parser.parse();
    UASM::Analyzer analyzer(std::move(program));
    analyzer.analyze();
    return 0;
}
