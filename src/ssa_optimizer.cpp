#include "../include/ssa_optimizer.h"

namespace UASM {

    SSAOptimizer::SSAOptimizer(std::unique_ptr<Program> _program) {
        this->program = std::move(_program);
    }

    void SSAOptimizer::optimize() {}

    void SSAOptimizer::visit_binary_expr(BinaryExpr& expr) {
        if ((expr.left->type == INTEGER_TOKEN && expr.right->type == INTEGER_TOKEN)
                || (expr.left->type == FLOAT_TOKEN && expr.right->type == FLOAT_TOKEN)) {
            assign_temp(expr.left);
        }
    }

    void SSAOptimizer::visit_assignment(Assignment& assignment) {
        Symbol& sym = assignment.identifier;
        Symbol& stored_sym = ctx.first->symbols[sym.variable->symbol];
        if (sym.variable != stored_sym.variable) {
            sym.variable->symbol = "t" + std::to_string(temps);
            ctx.first->insert_symbol(sym);
            temps++;
        }


    }

}
