#include "../include/semantics.h"

namespace UASM {
    Analyzer::Analyzer(std::unique_ptr<Program> _program) : program(std::move(_program)) {};

    void Analyzer::analyze() {
        for (auto& f : program->functions)
            visit_func(f.second);
    }

    void Analyzer::visit_func(Function& func) {

        for (auto& p : func.labels) {
            Label& label = p.second;
            visit_label(label);
        }
    }

    void Analyzer::visit_label(Label& label) {
        for (Instruction& inst : label.instructions) {
            if (std::holds_alternative<Assignment>(inst))
                visit_assignment(std::get<Assignment>(inst));
            else if (std::holds_alternative<JmpInst>(inst))
                visit_jmp( std::get<JmpInst>(inst));
            else
                visit_ret( std::get<Return>(inst));
        }
    }

    void Analyzer::visit_assignment(Assignment& inst) {
        if (ctx->symbols.count(inst.identifier.variable.symbol) > 0)
            std::logic_error("ssa violation - variable redeclaration");

        ctx->symbols.insert( { inst.identifier.variable.symbol, inst.identifier } );

        if (std::holds_alternative<BinaryExpr>(inst.expr))
            visit_binary_expr( std::get<BinaryExpr>(inst.expr));
        

    }

    void Analyzer::visit_binary_expr(BinaryExpr& expr) {
        TokenType t1;
        TokenType t2;
        switch(expr.op.type) {
            case MINUS_TOKEN:
            case PLUS_TOKEN:
            case DIV_TOKEN:
            case MUL_TOKEN:
                t1 = get_operand_type( &expr.left);
                t2 = get_operand_type( &expr.right);
                if (t1 != t2)
                    throw std::logic_error("incompatible types for this operation");
                expr_types[&expr] = t1;
                break;
            case BT_TOKEN:
            case BTE_TOKEN:
            case LT_TOKEN:
            case LTE_TOKEN:
            case NEQ_TOKEN:
            case DEQ_TOKEN:
                t1 = get_operand_type( &expr.left);
                t2 = get_operand_type( &expr.right);
                if (t1 != t2)
                    throw std::logic_error("comparison operations can only be applied on operands of the same type");
                expr_types[&expr] = I1_TYPE_TOKEN;
                break;

            default:
                throw std::logic_error("unsupported operation");

        }
    }

    void Analyzer::visit_literal(Token& literal) {
        if (ctx->symbols.count(literal.symbol) == 0)
            throw std::logic_error("undefined symbol");
        if (literal.type != 0)
            throw std::logic_error("assignment failed - incompatible types");
    }

    void Analyzer::visit_jmp(JmpInst& inst) {
        if (ctx->labels.count(inst.target.symbol) == 0)
            throw std::logic_error("not a valid jump target");
    }

    void Analyzer::visit_ret(Return& ret_val) {
        if (ctx->ret_type.type != ret_val.value.type)
            throw std::logic_error("returned type does not match the function's return type");
    }

    TokenType Analyzer::get_operand_type(Token* operand) {
        if (operand->type == IDENTIFIER_TOKEN) {
            Symbol& sym = ctx->symbols.at(operand->symbol);
            return sym.type.type;
        }
        
        return I1_TYPE_TOKEN;
    }


}
