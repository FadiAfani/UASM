#include "../include/parser.h"

namespace UASM {

    Parser::Parser(const std::vector<std::unique_ptr<UASM::Token>>& _tokens) : tokens(_tokens) {}

    void Parser::parse() {
        Token* token;
        while(cur_token < tokens.size())
            parse_label();
    }

    void Parser::parse_label() {
        Token* mod = consume_token(MOD_TOKEN, "labels start with a '%' symbol");
        Token* label = consume_token(IDENTIFIER_TOKEN, "expected a label");
        Token* colon = consume_token(COLON_TOKEN, "expected a ':' symbol");
        Token* t;

        if (label == nullptr || colon == nullptr || mod == nullptr)
            return;
        auto node = std::make_unique<Label>();
        node->name = label;

        while ((t = peek(0)) != nullptr && t->type != MOD_TOKEN) {
            std::optional<Instruction> inst = parse_instruction();
            if (inst.has_value())
                node->instructions.push_back(std::move(inst.value()));
        }

        labels.push_back(std::move(node));

        if (peek(0) != nullptr) {
            parse_label();
        }

    }

    std::optional<Instruction> Parser::parse_instruction() {
        Token* t = peek(0);
        std::optional<Instruction> inst;
        if (t == nullptr)
            return {};
        if (t->type == IDENTIFIER_TOKEN) 
            return parse_assignment();
        else
            return parse_jmp();
        return {};
    }

    std::optional<Assignment> Parser::parse_assignment() {
        Token* t;
        Assignment inst;

        inst.id = consume_token(IDENTIFIER_TOKEN, nullptr);
        t = consume_token(EQ_TOKEN, "missing '=' symbol");
        if (t == nullptr)
            return {};
        std::unique_ptr<Expr> expr = parse_expr();
        if (expr == nullptr)
            logger.log(t->col, t->row, "expected an expression after '='");
        
        inst.expr = std::move(expr);
        return inst;
        
    }

    std::optional<VarDef> Parser::parse_definition() {
        VarDef def;
        Token* t = consume_token(IDENTIFIER_TOKEN, "expected a register");
        if (t == nullptr)
            return {};
        def.type = t;
        t = consume_token(COLON_TOKEN, "expected a ':' symbol");
        if (t == nullptr)
            return {};
        t = consume_any(
                {
                    I8_TYPE_TOKEN,
                    I16_TYPE_TOKEN,
                    I32_TYPE_TOKEN,
                    I64_TYPE_TOKEN,
                    F32_TYPE_TOKEN,
                    F64_TYPE_TOKEN,
                    U8_TYPE_TOKEN,
                    U16_TYPE_TOKEN,
                    U32_TYPE_TOKEN,
                    U64_TYPE_TOKEN
                }
                , 
                "missing type");

        if (t == nullptr)
            return {};
        def.variable = t;

        return {};
        
    }

    std::optional<JmpInst> Parser::parse_jmp() {
        JmpInst inst;
        consume_token(GOTO_TOKEN, nullptr);
        Token* t = consume_token(IDENTIFIER_TOKEN, "goto instructions require at least a target label");
        if (t == nullptr)
            return {};
        inst.target = t; 
        if ((t = peek(0)) != nullptr && t->type == IDENTIFIER_TOKEN)
            inst.cond = t;

        return inst;


    }
    
    std::unique_ptr<Expr> Parser::parse_expr() {
        Token* t1 = peek(0);
        Token* t2 = peek(1);
        auto expr = std::make_unique<Expr>();
        if (t1 && t2 && t1->type == IDENTIFIER_TOKEN) {
            std::optional<BinaryExpr> bin = parse_binary_expr();
            if (bin.has_value())
                *expr = bin.value();
        } else if (t1 && t1->type == IDENTIFIER_TOKEN) {
            expr = t1;
        }
    }

    std::optional<BinaryExpr> Parser::parse_binary_expr() {
        BinaryExpr expr;
        expr.left = consume_any({IDENTIFIER_TOKEN, FLOAT_TOKEN, INTEGER_TOKEN}, nullptr);
        expr.op = consume_any(
            {
            PLUS_TOKEN, 
            MINUS_TOKEN, 
            MUL_TOKEN, 
            DIV_TOKEN, 
            RSHIFT_TOKEN, 
            LSHIFT_TOKEN, 
            MOD_TOKEN, 
            LT_TOKEN, 
            LTE_TOKEN, 
            BT_TOKEN, 
            BTE_TOKEN,
            DEQ_TOKEN,
            NEQ_TOKEN
            }, nullptr);
        
        expr.right = consume_any({IDENTIFIER_TOKEN, FLOAT_TOKEN, INTEGER_TOKEN}, "expected an operand after operation");
        if (expr.right == nullptr)
            return {};

        return expr;
    };

    Token* Parser::peek(size_t n) {
        if (cur_token + n >= tokens.size())
            return nullptr;

        return tokens.at(cur_token + n).get();

    }

    Token* Parser::consume_token(TokenType type, const char* err_msg) {
        Token* t = get_next_token();
        if (t == nullptr) {
            Token* last = tokens.back().get();
            logger.log(last->col, last->row, err_msg);
            return nullptr;
        }
        if (t->type != type)
            logger.log(t->col, t->row, err_msg);

        this->cur_token++;
        return t;
    }

    Token* Parser::consume_any(std::initializer_list<TokenType> types, const char* err_msg) {
        Token* token; 
        for (TokenType t : types) {
            token = consume_token(t, err_msg);
            if (token != nullptr)
                return token;
        }
        return nullptr;
    }

    Token* Parser::get_next_token() {
        if (cur_token < tokens.size())
            return tokens.at(cur_token).get();


        return nullptr;
    }

    const std::vector<std::unique_ptr<Error>>& Parser::get_errors() { return logger.get_errors(); }

    const std::vector<std::unique_ptr<Label>>& Parser::get_labels() {
        return labels;
    }

}

