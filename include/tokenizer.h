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
        PLUS_TOKEN,
        MINUS_TOKEN,
        MUL_TOKEN,
        DIV_TOKEN,
        LSHIFT_TOKEN,
        RSHIFT_TOKEN,
        LT_TOKEN,
        LTE_TOKEN,
        BT_TOKEN,
        BTE_TOKEN,
        MOD_TOKEN,
        DEQ_TOKEN,
        NEQ_TOKEN,
        EQ_TOKEN,
        AT_TOKEN,
        COLON_TOKEN,
        COMMA_TOKEN,
        I1_TYPE_TOKEN,
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
        GOTO_TOKEN,
        LPAREN_TOKEN,
        RPAREN_TOKEN,
        LCURLY_TOKEN,
        RCURLY_TOKEN,
        RET_TOKEN,
        EXTERN_TOKEN

    };


    static const std::unordered_map<std::string, TokenType> reserved_words = {
        { "extern", EXTERN_TOKEN },
        { "goto", GOTO_TOKEN },
        { "ret",  RET_TOKEN  },
        { "i1" ,  I1_TYPE_TOKEN },
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

        Token() = default;

        Token (Token&& other) noexcept : col(other.col), row(other.row), symbol(std::move(other.symbol)), type(other.type) {}
        Token (Token& other) : col(other.col), row(other.row), symbol(std::move(other.symbol)), type(other.type) {}

        Token& operator=(Token& other) {
            if (this != &other) {
                col = other.col;
                row = other.row;
                symbol = std::move(other.symbol);
                type = other.type;
            }
            return *this;
        }

        Token& operator=(Token&& other) noexcept {
            if (this != &other) {
                col = other.col;
                row = other.row;
                symbol = std::move(other.symbol);
                type = other.type;
            }

            return *this;
        }
    };

    class Tokenizer {
        private:
            std::vector<Token> tokens;
            size_t col = 0;
            size_t row = 0;
            std::vector<std::unique_ptr<Error>> errors;


        public:
            Tokenizer();
            void tokenize(const char* filename);
            void handle_simple_token(std::string symbol, TokenType type);
            std::vector<Token>& get_tokens();
            const std::vector<std::unique_ptr<Error>>& get_errors();
    };

}



#endif 
