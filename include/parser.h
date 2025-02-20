#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <optional>
#include <variant>
#include "../include/tokenizer.h"

namespace UASM {

    

    struct Symbol {
        Token* variable;
        Token* type;
        unsigned int reg;
    };

    typedef std::variant<Symbol, Token*> Operand;

    struct BinaryExpr {
        Token* left;
        Token* op;
        Token* right;
    };

    struct FuncCall {
        Token* id;
        std::vector<std::reference_wrapper<Token>> args;
    };

    struct UnaryExpr {
        Token* op;
        Token* value;

    };

    typedef std::variant<BinaryExpr, Token*> Expr;

    struct Assignment {
        Symbol identifier;
        Expr expr;

    };

    struct JmpInst {
        Token* cond;
        Token* target;
    };

    typedef std::variant<Assignment, JmpInst, Token*> Instruction;

    struct Label {
        Token* name;
        std::vector<Instruction> instructions;
    };

    struct Function {
        Token* name;
        Token* ret_type;
        std::unordered_map<std::string, Label> labels;
        std::unordered_map<std::string, Symbol> symbols;
    };


    struct Program {
        std::unordered_map<std::string, Function> functions;
        std::unordered_map<std::string, Token*> externs;
    };

    class Parser {
        private:
            std::vector<Token>& tokens;
            std::vector<std::unique_ptr<Error>> errors;
            std::unique_ptr<Program> program = nullptr;
            size_t cur_token = 0;
        
        public:
            Parser(std::vector<Token>& _tokens);
            Token* consume_any(std::initializer_list<TokenType> types, const char* err_msg);
            Token* consume_token(TokenType type, const char* err_msg);
            Token* get_next_token();
            Token* peek(size_t n);
            Token* parse_binary_op(const char* err_msg);
            Token* parse_type(const char* err_msg);
            Token* parse_literal(const char* err_msg);
            std::unique_ptr<Program> parse();
            void parse_stmt();
            void parse_extern_stmt();
            void parse_function();
            void recover(std::initializer_list<TokenType> safe_tokens);
            std::optional<Label> parse_label(Function& func);
            std::optional<Instruction> parse_instruction(Function& func);
            std::optional<Assignment> parse_assignment(Function& func);
            std::optional<JmpInst> parse_jmp();
            Token* parse_ret();
            void parse_func_call();
            std::optional<BinaryExpr> parse_binary_expr();
            std::optional<Expr> parse_expr();
            std::optional<Symbol> parse_definition();
            void get_parser_errors();
            std::unordered_map<std::string, Function>& get_functions();
            const std::vector<std::unique_ptr<Error>>& get_errors();

    };

}


#endif
