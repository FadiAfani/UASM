#include "../include/ssa_optimizer.h"
#include "../include/visitor.h"

namespace UASM {

    SSAOptimizer::SSAOptimizer(CFGData* _cd) : CFGOptimizer(_cd) {}

    void SSAOptimizer::visit_literal(Token& literal) { 
        if (literal.type == IDENTIFIER_TOKEN)
            literal.symbol = dups[literal.symbol].new_symbol;
    }
    void SSAOptimizer::visit_jmp(JmpInst& inst) {
    }
    void SSAOptimizer::visit_label(Label& label) {
    }
    void SSAOptimizer::visit_func(Function& function) {
    }
    void SSAOptimizer::visit_ret(Return& ret) {

    }
    void SSAOptimizer::visit_binary_expr(BinaryExpr& expr) {
    }

    void SSAOptimizer::visit_assignment(Assignment& assignment) {
        Expr& expr = assignment.expr;
        std::string& old_symbol = assignment.identifier.variable.symbol;

        if (dups.count(old_symbol) > 0) {
            std::string new_symbol = old_symbol + std::to_string(dups[old_symbol].dups++);
            dups[old_symbol].new_symbol = new_symbol;
        } else {
            dups[old_symbol] = { .dups = 0, .new_symbol = old_symbol + "0" };
        }

        if (std::holds_alternative<BinaryExpr>(expr)) {
            BinaryExpr& bin = std::get<BinaryExpr>(expr);
            if ((bin.left.type == INTEGER_TOKEN || bin.left.type == FLOAT_TOKEN)
                    && (bin.right.type == INTEGER_TOKEN || bin.right.type == FLOAT_TOKEN)) {
                Token temp;
                temp.type = IDENTIFIER_TOKEN;
                temp.symbol = "$" + std::to_string(temps++);
                Assignment temp_assign;
                temp_assign.expr = bin.left;
                temp_assign.identifier = { .variable = std::move(temp) };

                for (std::list<Instruction>::iterator it = ctx->instructions.begin(); it != ctx->instructions.end(); it++) {
                    if (std::holds_alternative<Assignment>(*it)) {
                        Assignment& node_value = std::get<Assignment>(*it);
                        if (&node_value == &assignment) {
                            ctx->instructions.insert(it, temp_assign);
                        }

                    }
                    
                }

            }

        }
    }

    void SSAOptimizer::optimize() {
        for (auto&[_, bbs] : cd->cfgs) {
            if (bbs.size() > 0) {
                for ( BasicBlock::InOrder it(bbs.front().get()); !it.empty(); ++it) {
                    it->accept(*this);
                }
            }
        }
    }

}
