#include "../include/parser.h"

namespace UASM {
    /* WARNING: this is a very bad parser - REFACTOR */

    /* NOTE: move this to the Token Class */
    static bool is_binary_op(TokenType t) {
        return (
            t == PLUS_TOKEN 
            || t == MINUS_TOKEN 
            || t == DIV_TOKEN
            || t == MUL_TOKEN
            || t == MOD_TOKEN
            || t == RSHIFT_TOKEN
            || t == LSHIFT_TOKEN
            || t == LT_TOKEN
            || t == LTE_TOKEN
            || t == BT_TOKEN
            || t == BTE_TOKEN
            || t == NEQ_TOKEN
            || t == DEQ_TOKEN
            );
    }

    Parser::Parser(std::vector<UASM::Token>& _tokens) : tokens(_tokens) {}

    void Parser::parse() {
        Token* t = peek(0);
        while(t != nullptr && t->type == AT_TOKEN)
            parse_function();
    }

    void Parser::parse_function() {
        Function func;
        consume_token(AT_TOKEN, nullptr);
        Token* name = consume_token(IDENTIFIER_TOKEN, "expected an identifier");
        Token* lparen = consume_token(LPAREN_TOKEN, "expected a '(' symbol");
        if (name == nullptr || lparen == nullptr)
            return;
        std::optional<VarDef> p = parse_definition();
        if (p.has_value()) {
            Token* comma;
            func.params.push_back(p.value());
            while ( (comma = consume_token(COMMA_TOKEN, nullptr) ) != nullptr && comma->type == COMMA_TOKEN) {
                p = parse_definition();
                func.params.push_back(p.value());
            }
            Token* rparen = consume_token(RPAREN_TOKEN, "expected a ')' symbol");
            Token* colon  = consume_token(COLON_TOKEN, "expected a ':' symbol");
            func.ret_type = parse_type();
            Token* lcurly = consume_token(LCURLY_TOKEN, "expected a '{' symbol");
            Token* mod;
            while((mod = peek(0)) != nullptr && mod->type == MOD_TOKEN) {
                std::optional<Label> label = parse_label();
                if (label.has_value())
                    func.labels.insert({ label->name->symbol, label.value() });
            }
            Token* rcurly = consume_token(RCURLY_TOKEN, "expected a '}' symbol");
            if (rcurly == nullptr)
                return;
            functions.insert({name->symbol, std::move(func)});

        }
    }

    std::optional<Label> Parser::parse_label() {
        Token* mod = consume_token(MOD_TOKEN, "labels start with a '%' symbol");
        Token* name = consume_token(IDENTIFIER_TOKEN, "expected a label");
        Token* colon = consume_token(COLON_TOKEN, "expected a ':' symbol");
        Token* t;

        if (name == nullptr || colon == nullptr || mod == nullptr)
            return {};
        Label label;
        label.name = name;

        while ((t = peek(0)) != nullptr && t->type != MOD_TOKEN) {
            std::optional<Instruction> inst = parse_instruction();
            if (inst.has_value())
                label.instructions.push_back(std::move(inst.value()));
        }

        return label;

    }

    std::optional<Instruction> Parser::parse_instruction() {
        Token* t = peek(0);
        std::optional<Instruction> inst;
        if (t == nullptr)
            return {};
        if (t->type == IDENTIFIER_TOKEN) 
            return parse_assignment();
        else if (t->type == GOTO_TOKEN)
            return parse_jmp();
        return {};
    }

    std::optional<Assignment> Parser::parse_assignment() {
        Token* t;
        Assignment inst;

        std::optional<VarDef> def = parse_definition(); 
        if (!def.has_value())
            return {};
        t = consume_token(EQ_TOKEN, "missing '=' symbol");
        if (t == nullptr)
            return {};
        auto expr = parse_expr();
        if (!expr.has_value()) {
            logger.log(t->col, t->row, "expected an expression after '='");
            return {};
        }
        
        inst.expr = expr.value(); 
        return inst;
        
    }

    std::optional<VarDef> Parser::parse_definition() {
        VarDef def;
        Token* t = consume_token(IDENTIFIER_TOKEN, "expected a register");
        if (t == nullptr)
            return {};
        def.variable = t;
        t = consume_token(COLON_TOKEN, "expected a ':' symbol");
        if (t == nullptr)
            return {};
        t = parse_type();

        if (t == nullptr) {
            logger.log(def.variable->col, def.variable->row, "missing type");
            return {};
        }
        def.variable = t;

        return def;
        
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
    
    std::optional<Expr> Parser::parse_expr() {
        Token* t1 = peek(0);
        Token* t2 = peek(1);
        Expr expr;
        
        if (t2 && is_binary_op(t2->type)) {
            std::optional<BinaryExpr> bin = parse_binary_expr();
            if (bin.has_value())
                expr = bin.value();
        } else if (t1 && t1->type == IDENTIFIER_TOKEN) {
            expr = t1;
        }
        return expr;
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

        return &tokens[cur_token + n];

    }

    Token* Parser::consume_token(TokenType type, const char* err_msg) {
        Token* t = get_next_token();
        if (t == nullptr) {
            const Token& last = tokens.back();
            logger.log(last.col, last.row, err_msg);
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
            if ((token = peek(0)) && token->type == t) {
                cur_token++;
                return token;
            }
            
        }
        Token& last = tokens.back();
        logger.log(last.col, last.row, err_msg);
        return nullptr;
    }

    Token* Parser::get_next_token() {
        if (cur_token < tokens.size())
            return &tokens[cur_token];


        return nullptr;
    }

    Token* Parser::parse_type() {
        Token* t = peek(0);
        if (t == nullptr 
                || t->type != I8_TYPE_TOKEN
                || t->type != I16_TYPE_TOKEN
                || t->type != I32_TYPE_TOKEN
                || t->type != I64_TYPE_TOKEN
                || t->type != U8_TYPE_TOKEN
                || t->type != U16_TYPE_TOKEN
                || t->type != U32_TYPE_TOKEN
                || t->type != U64_TYPE_TOKEN
                || t->type != F32_TYPE_TOKEN
                || t->type != F64_TYPE_TOKEN
                )
            return nullptr;
        return t;
    }

    const std::vector<std::unique_ptr<Error>>& Parser::get_errors() { return logger.get_errors(); }

    const std::unordered_map<std::string, Function>& Parser::get_functions() {
        return functions;
    }

}

