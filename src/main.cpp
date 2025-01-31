#include "../include/tokenizer.h"
#include "../include/parser.h"

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "specify a filepath argument" << std::endl;
    }
    UASM::Tokenizer tokenizer;
    UASM::Parser parser(tokenizer.get_tokens());
    tokenizer.tokenize(argv[1]);
    parser.parse();
    std::cout << parser.get_errors().size() << std::endl;
    for (auto& err : parser.get_errors())
        err->print();
    for (auto& t : parser.get_labels()) 
        std::cout << t->name->symbol << std::endl;
   
    return 0;
}
