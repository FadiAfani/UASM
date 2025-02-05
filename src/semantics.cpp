#include "../include/semantics.h"

namespace UASM {
    Analyzer::Analyzer(const std::unordered_map<std::string, Function>& _functions, std::unordered_map<std::string, Symbol>& _symbols) : functions(_functions), symbols(_symbols) {};

    void Analyzer::verify_func(Function& func) {
        for (auto& p : func.labels) {
            Label& label = p.second;
            verify_label(func, label);
        }
    }

    void Analyzer::verify_label(Function& func, Label& label) {
        for (Instruction& inst : label.instructions) {
            if (std::holds_alternative<Assignment>(inst))
                verify_assignment(func, std::get<Assignment>(inst));
            else if (std::holds_alternative<JmpInst>(inst))
                verify_jmp(func, std::get<JmpInst>(inst));
            else
                verify_ret(func, std::get<Token*>(inst));
        }
    }

    void Analyzer::verify_assignment(Function& func, Assignment& inst) {
        if (std::holds_alternative<BinaryExpr>(inst.expr))
            verify_binary_expr(func, std::get<BinaryExpr>(inst.expr), inst.identifier.type->type);
        else 
            verify_literal(func, std::get<Token*>(inst.expr), inst.identifier.type->type);
        

    }

    void Analyzer::verify_binary_expr(Function& func, BinaryExpr& expr, TokenType expected_type) {
        switch(expr.op->type) {
            case MINUS_TOKEN:
            case PLUS_TOKEN:
            case DIV_TOKEN:
            case MUL_TOKEN:
                if (expr.left->type != expr.right->type 
                        || (expr.left->type == expr.right->type && expr.left->type == expected_type))
                    throw std::logic_error("incompatible types for this operation");
                break;
            case BT_TOKEN:
            case BTE_TOKEN:
            case LT_TOKEN:
            case LTE_TOKEN:
            case NEQ_TOKEN:
            case DEQ_TOKEN:
                if (expr.left->type != expr.right->type)
                    throw std::logic_error("comparison operations can only be applied on operands of the same type");
                else if (expected_type != I1_TYPE_TOKEN)
                    throw std::logic_error("comparison operation produce a value of type 'i1'");
                break;

            default:
                throw std::logic_error("unsupported operation");

        }
    }

    void Analyzer::verify_literal(Function& func, Token* literal, TokenType expected_type) {
        if (symbols.count(literal->symbol) == 0)
            throw std::logic_error("undefined symbol");
        if (literal->type != expected_type)
            throw std::logic_error("assignment failed - incompatible types");
    }

    void Analyzer::verify_jmp(Function& func, JmpInst& inst) {
        if (func.labels.count(inst.target->symbol) == 0)
            throw std::logic_error("not a valid jump target");
    }

    void Analyzer::verify_ret(Function& func, Token* ret_val) {
        if (func.ret_type->type != ret_val->type)
            throw std::logic_error("returned type does not match the function's return type");
    }

}
