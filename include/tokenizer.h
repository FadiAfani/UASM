#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <unordered_map>

#include "error.h"


namespace UASM {



    enum TokenType {
        MNEMONIC_TOKEN,
        COLON_TOKEN,
        COMMA_TOKEN,
        ADDI_OPCODE_TOKEN,
        ADD_OPCODE_TOKEN,
        I8_TYPE_TOKEN,
        I16_TYPE_TOKEN,
        I32_TYPE_TOKEN,
        I64_TYPE_TOKEN,
        F32_TYPE_TOKEN,
        F64_TYPE_TOKEN,
        U8_TYPE_TOKEN,
        U16_TYPE_TOKEN,
        U32_TYPE_TOKEN,
        U64_TYPE_TOKEN,
        IDENTIFIER_TOKEN,
        INTEGER_TOKEN,
        FLOAT_TOKEN,

    };

    static const std::unordered_map<const char*, TokenType> reserved_words = {
        // 
        { "addi", ADDI_OPCODE_TOKEN },
        { "i8",   I8_TYPE_TOKEN },
        { "i16",  I16_TYPE_TOKEN },
        { "i32",  I32_TYPE_TOKEN },
        { "i64",  I64_TYPE_TOKEN },
        { "f32",  F32_TYPE_TOKEN },
        { "f64",  F64_TYPE_TOKEN },
        { "u8",   U8_TYPE_TOKEN },
        { "u16",  U16_TYPE_TOKEN },
        { "u32",  U32_TYPE_TOKEN },
        { "u64",  U64_TYPE_TOKEN },
    };

    struct Token {
        size_t col;
        size_t row;
        std::string symbol;
        TokenType type;
    };

    class Tokenizer {
        private:
            std::vector<std::unique_ptr<Token>> tokens;
            size_t col = 0;
            size_t row = 0;
            std::vector<std::unique_ptr<Error>> errors;


        public:
            Tokenizer();
            void tokenize(const char* filename);
            const std::vector<std::unique_ptr<Token>>& get_tokens();
            const std::vector<std::unique_ptr<Error>>& get_errors();
    };

}



#endif 
