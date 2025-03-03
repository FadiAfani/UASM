#include "../include/visitor.h"
#include <iostream>

namespace UASM {

    void BinaryExpr::print() {}
    void BinaryExpr::accept(ProgramVisitor& visitor) {}

    void FuncCall::print() {}
    void FuncCall::accept(ProgramVisitor& visitor) {}

    void UnaryExpr::print() {}
    void UnaryExpr::accept(ProgramVisitor& visitor) {}

    void Assignment::print() { std::cout << "<Assignment>" << std::endl; }
    void Assignment::accept(ProgramVisitor& visitor) {}

    void JmpInst::print() { std::cout << "<Goto>" << std::endl; }
    void JmpInst::accept(ProgramVisitor& visitor) {}

    void Label::print() {}
    void Label::accept(ProgramVisitor& visitor) {}

    void Function::print() {}
    void Function::accept(ProgramVisitor& visitor) {}

    void Return::print() { std::cout << "<Return>" << std::endl; }
    void Return::accept(ProgramVisitor& visitor) {}

    Return::Return(Token _val) : value(_val) {}

    void Function::insert_symbol(Symbol sym) {
        if (symbols.count(sym.variable.symbol) == 0)
            symbols[sym.variable.symbol] = sym;
    }

    void Program::print() {}
    void Program::accept(ProgramVisitor& visitor) {}

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
        }
        printf("\n-----------\n");
    }

    void BasicBlock::accept(ProgramVisitor& visitor) {
        for (Instruction& inst : instructions) {
            std::visit([&visitor](auto&& i) {
                    i.accept(visitor);
            }, inst);
        }
        for (BasicBlock* bb : successors)
            bb->accept(visitor);
    }

}
