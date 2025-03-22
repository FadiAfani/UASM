#include "../include/visitor.h"
#include <iostream>
#include <variant>

namespace UASM {

    void Token::print() { std::cout << symbol; }
    void Token::accept(ProgramVisitor& visitor) { visitor.visit_literal(*this); }

    void BinaryExpr::print() {
        left.print();
        std::cout << " ";
        op.print();
        std::cout << " ";
        right.print();
    }
    void BinaryExpr::accept(ProgramVisitor& visitor) {
        visitor.visit_binary_expr(*this);
    }

    void FuncCall::print() {}
    void FuncCall::accept(ProgramVisitor& visitor) {}

    void UnaryExpr::print() {}
    void UnaryExpr::accept(ProgramVisitor& visitor) {}

    void Assignment::print() {
        identifier.variable.print();
        std::cout << ": ";
        identifier.type.print();
        std::cout << " = ";
        std::visit([](auto&& e) { e.print(); }, expr);
    }
    void Assignment::accept(ProgramVisitor& visitor) {
        visitor.visit_assignment(*this);
    }

    void JmpInst::print() {
        std::cout << "goto ";
        target.print();
        if (cond.has_value())
            cond.value().print();
    }
    void JmpInst::accept(ProgramVisitor& visitor) {
        visitor.visit_jmp(*this);
    }

    void Label::print() {}
    void Label::accept(ProgramVisitor& visitor) {
        visitor.visit_label(*this);
    }

    void Function::print() {}
    void Function::accept(ProgramVisitor& visitor) {
        visitor.visit_func(*this);
    }

    void Return::print() { 
        std::cout << "return ";
        value.print();
    }
    void Return::accept(ProgramVisitor& visitor) {
        value.accept(visitor);
    }
    

    Return::Return(Token _val) : value(_val) {}

    void Function::insert_symbol(Symbol sym) {
        if (symbols.count(sym.variable.symbol) == 0)
            symbols[sym.variable.symbol] = sym;
    }

    void Program::print() {}
    void Program::accept(ProgramVisitor& visitor) {
        for (auto&[_, f] : functions) {
            f.accept(visitor);
        }

    }

    BasicBlock::BasicBlock(Function& _pf) : pf(_pf) {}

    void BasicBlock::print() {
        printf("\n-----------\n");
        printf("BasicBlock: %ld\n", tag);
        printf("Successors: ");
        for (BasicBlock* bb : successors) {
            printf("%ld-> ", bb->tag);
        }
        printf("\n");
        printf("Instructions: \n");
        for (Instruction& inst : instructions) {
            std::visit([](auto&& i) {
                    i.print();
            }, inst);
            printf("\n");
        }
        printf("\n-----------\n");
    }

    void BasicBlock::accept(ProgramVisitor& visitor) {
        for (Instruction& inst : instructions) {
            visitor.visit_instruction(inst);
        }
    }

    std::unordered_set<BasicBlock*> compute_idoms() {
        std::unordered_set<BasicBlock*> idoms;


        return idoms;
    }

}
