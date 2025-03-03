#include "../include/ssa_optimizer.h"

namespace UASM {

    void SSAOptimizer::visit_literal(Token& literal) { 
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

        if (dups.count(&assignment.identifier) > 0) {
            assignment.identifier.variable.symbol += std::to_string(dups[&assignment.identifier]++);
        } else {
            dups[&assignment.identifier] = 0;
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


}
