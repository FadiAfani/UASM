#ifndef PARSER_H
#define PARSER_H

#include <cstddef>
#include <memory>
#include <optional>
#include <variant>
#include <queue>
#include <list>
#include "tokenizer.h"
#include "visitor.h"

namespace UASM {

    struct Unit {
        virtual ~Unit() {}
        virtual void print() = 0;
        virtual void accept(Visitor& visitor) = 0;

    };

    struct Symbol {
        Token variable;
        Token type;
        unsigned int reg;
    };

    struct BinaryExpr : Unit {
        Token left;
        Token op;
        Token right;

        void print() override;
        void accept(Visitor& visitor) override;
    };

    struct FuncCall : Unit {
        Token id;
        std::vector<std::reference_wrapper<Token>> args;

        void print() override;
        void accept(Visitor& visitor) override;
    };

    struct UnaryExpr : Unit {
        Token op;
        Token value;

        void print() override;
        void accept(Visitor& visitor) override;

    };

    typedef std::variant<BinaryExpr, Token*> Expr;

    struct Assignment : Unit {
        Symbol identifier;
        Expr expr;

        void print() override;
        void accept(Visitor& visitor) override;

    };

    struct JmpInst : Unit {
        Token cond;
        Token target;

        void print() override;
        void accept(Visitor& visitor) override;
    };

    struct Return : Unit {
        Token value;

        Return(Token _value);

        void print() override;
        void accept(Visitor& visitor) override;
    };

    typedef std::variant<Assignment, JmpInst, Return, FuncCall> Instruction;

    struct Label : Unit {
        Token name;
        std::vector<Instruction> instructions;

        void print() override;
        void accept(Visitor& visitor) override;
    };

    struct Function : Unit {
        Token name;
        Token ret_type;
        std::unordered_map<std::string, Label> labels;
        std::unordered_map<std::string, Symbol> symbols;

        void print() override;
        void accept(Visitor& visitor) override;
        void insert_symbol(Symbol sym);
    };

    struct Program : Unit {
        std::unordered_map<std::string, Function> functions;
        std::unordered_map<std::string, Token*> externs;

        void print() override;
        void accept(Visitor& visitor) override;

    };

    /* NOTE:
     * keep optonals for future-proofing
     * at this stage optionals are redundant since consume_token throws an error
     *
     * */
    
    class Parser {
        private:
            std::vector<Token>& tokens;
            std::vector<std::unique_ptr<Error>> errors;
            std::unique_ptr<Program> program = nullptr;
            size_t cur_token = 0;
        
        public:
            Parser(std::vector<Token>& _tokens);
            std::optional<Token> consume_any(std::initializer_list<TokenType> types, const char* err_msg);
            std::optional<Token> consume_token(TokenType type, const char* err_msg);
            std::optional<Token> get_next_token();
            Token* peek(size_t n);
            std::optional<Token> parse_binary_op(const char* err_msg);
            std::optional<Token> parse_type(const char* err_msg);
            std::optional<Token> parse_literal(const char* err_msg);
            std::unique_ptr<Program> parse();
            void parse_stmt();
            void parse_extern_stmt();
            void parse_function();
            void recover(std::initializer_list<TokenType> safe_tokens);
            std::optional<Label> parse_label(Function& func);
            std::optional<Instruction> parse_instruction(Function& func);
            std::optional<Assignment> parse_assignment(Function& func);
            std::optional<JmpInst> parse_jmp();
            std::optional<Return> parse_ret();
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
