#include "../include/tokenizer.h"

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "specify a filepath argument" << std::endl;
    }
    Tokenizer tokenizer;
    tokenizer.tokenize(argv[1]);
    return 0;
}