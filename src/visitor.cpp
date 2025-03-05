#include "../include/visitor.h"
#include <iostream>

namespace UASM {

    void Token::print() { std::cout << "<Token>" << std::endl; };
    void Token::accept(ProgramVisitor& visitor) { visitor.visit_literal(*this); }

    void BinaryExpr::print() {}
    void BinaryExpr::accept(ProgramVisitor& visitor) {
        visitor.visit_binary_expr(*this);
        left.accept(visitor);
        right.accept(visitor);
    }

    void FuncCall::print() {}
    void FuncCall::accept(ProgramVisitor& visitor) {}

    void UnaryExpr::print() {}
    void UnaryExpr::accept(ProgramVisitor& visitor) {}

    void Assignment::print() { std::cout << "<Assignment>" << std::endl; }
    void Assignment::accept(ProgramVisitor& visitor) {
        visitor.visit_assignment(*this);
        if (std::holds_alternative<BinaryExpr>(expr)) {
            std::get<BinaryExpr>(expr).accept(visitor);
        } else {
            std::get<Token>(expr).accept(visitor);
        }
    }

    void JmpInst::print() { std::cout << "<Goto>" << std::endl; }
    void JmpInst::accept(ProgramVisitor& visitor) {
        visitor.visit_jmp(*this);
        if (cond.has_value())
            cond.value().accept(visitor);
    }

    void Label::print() {}
    void Label::accept(ProgramVisitor& visitor) {
        visitor.visit_label(*this);
        for (Instruction& inst : instructions) {
            std::visit([&visitor](auto&& i) {
                i.accept(visitor);
            }, inst);
        }
    }

    void Function::print() {}
    void Function::accept(ProgramVisitor& visitor) {
        visitor.visit_func(*this);
        for (auto& [_, l] : labels) {
            l.accept(visitor);
        }
    }

    void Return::print() { std::cout << "<Return>" << std::endl; }
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
        }
        printf("\n-----------\n");
    }

    void BasicBlock::accept(ProgramVisitor& visitor) {
        for (Instruction& inst : instructions) {
            visitor.visit_instruction(inst);
        }
        for (BasicBlock* bb : successors)
            bb->accept(visitor);
    }

}
