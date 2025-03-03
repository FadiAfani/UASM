#ifndef VISIOR_H
#define VISIOR_H

#include "tokenizer.h"
#include <list>
#include <variant>



namespace UASM {

    class ProgramVisitor;

    struct Unit {
        virtual ~Unit() {}
        virtual void print() = 0;
        virtual void accept(ProgramVisitor& visitor) = 0;

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
        void accept(ProgramVisitor& visitor) override;
    };

    struct FuncCall : Unit {
        Token id;
        std::vector<std::reference_wrapper<Token>> args;

        void print() override;
        void accept(ProgramVisitor& visitor) override;
    };

    struct UnaryExpr : Unit {
        Token op;
        Token value;

        void print() override;
        void accept(ProgramVisitor& visitor) override;

    };

    typedef std::variant<BinaryExpr, Token> Expr;

    struct Assignment : Unit {
        Symbol identifier;
        Expr expr;

        void print() override;
        void accept(ProgramVisitor& visitor) override;

    };

    struct JmpInst : Unit {
        Token cond;
        Token target;

        void print() override;
        void accept(ProgramVisitor& visitor) override;
    };

    struct Return : Unit {
        Token value;

        Return(Token _value);

        void print() override;
        void accept(ProgramVisitor& visitor) override;
    };

    typedef std::variant<Assignment, JmpInst, Return, FuncCall> Instruction;


    struct Label : Unit {
        Token name;
        std::vector<Instruction> instructions;

        void print() override;
        void accept(ProgramVisitor& visitor) override;
    };

    struct Function : Unit {
        Token name;
        Token ret_type;
        std::unordered_map<std::string, Label> labels;
        std::unordered_map<std::string, Symbol> symbols;

        void print() override;
        void accept(ProgramVisitor& visitor) override;
        void insert_symbol(Symbol sym);
    };

    struct BasicBlock : Unit {
        Function& pf;
        size_t tag;
        std::list<Instruction> instructions;
        std::list<BasicBlock*> successors;

        BasicBlock(Function& _pf);

        void print() override;
        void accept(ProgramVisitor& visitor) override;
    };

    struct Program : Unit {
        std::unordered_map<std::string, Function> functions;
        std::unordered_map<std::string, Token*> externs;

        void print() override;
        void accept(ProgramVisitor& visitor) override;

    };

    class InstructionVisitor {
        public:
            virtual void visit_binary_expr(BinaryExpr& expr) = 0;
            virtual void visit_literal(Token& literal) = 0;
            virtual void visit_assignment(Assignment& inst) = 0;
            virtual void visit_jmp(JmpInst& inst) = 0;
            virtual void visit_ret(Return& ret_val) = 0;

    };

    class ProgramVisitor : InstructionVisitor {
        public:
            virtual ~ProgramVisitor() {}
            virtual void visit_label(Label& label) = 0;
            virtual void visit_func(Function& func) = 0;
    };

}



#endif
