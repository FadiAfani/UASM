#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <optional>
#include <variant>
#include <optional>
#include "../include/tokenizer.h"

namespace UASM {

    struct VarDef {
        Token* variable;
        Token* type;
    };

    typedef std::variant<VarDef, Token*> Operand;

    struct BinaryExpr {
        Token* left;
        Token* op;
        Token* right;
        
    };

    struct FuncCall {
        Token* id;
        std::vector<Token*> args;
    };

    struct UnaryExpr {
        Token* op;
        Token* value;
    };

    typedef std::variant<BinaryExpr, UnaryExpr, Token*> Expr;

    struct Assignment {
        Token* id;
        std::unique_ptr<Expr> expr;

        Assignment();

        Assignment(Assignment& other) : id(other.id), expr(std::move(other.expr))  {};
        Assignment(Assignment&& other) noexcept : id(other.id), expr(std::move(other.expr))  {};

        Assignment& operator=(Assignment& other) {
            if (this != &other) {
                id = other.id;
                expr = std::move(other.expr);
            }
            return *this;
        }

        Assignment& operator=(Assignment&& other) noexcept {
            if (this != &other) {
                id = other.id;
                expr = std::move(other.expr);
            }
            return *this;
        }
    };

    struct JmpInst {
        Token* cond;
        Token* target;
    };

    typedef std::variant<Assignment, JmpInst> Instruction;

    struct Label {
        Token* name;
        std::vector<Instruction> instructions;
    };

    class Parser {
        private:
            const std::vector<std::unique_ptr<Token>>& tokens;
            std::vector<std::unique_ptr<Error>> errors;
            std::vector<std::unique_ptr<Label>> labels;
            ErrorLogger logger;
            size_t cur_token = 0;
        
        public:
            Parser(const std::vector<std::unique_ptr<Token>>& _tokens);
            Token* consume_any(std::initializer_list<TokenType> types, const char* err_msg);
            Token* consume_token(TokenType type, const char* err_msg);
            Token* get_next_token();
            Token* peek(size_t n);
            void parse();
            void parse_label();
            std::optional<Instruction> parse_instruction();
            std::optional<Assignment> parse_assignment();
            std::optional<JmpInst> parse_jmp();
            void parse_func_call();
            std::optional<BinaryExpr> parse_binary_expr();
            std::unique_ptr<Expr> parse_expr();
            std::optional<VarDef> parse_definition();
            void get_parser_errors();
            const std::vector<std::unique_ptr<Label>>& get_labels();
            const std::vector<std::unique_ptr<Error>>& get_errors();

    };

}


#endif
