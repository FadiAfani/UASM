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
        Token* t;
        while((t = peek(0)) != nullptr && t->type == AT_TOKEN)
            parse_function();
    }

    void Parser::parse_function() {
        Function func;
        consume_token(AT_TOKEN, "");
        func.name = peek(0);
        consume_token(IDENTIFIER_TOKEN, "expected an identifier");
        consume_token(LPAREN_TOKEN, "expected a '(' symbol");
        std::optional<Symbol> p = parse_definition();
        if (p.has_value()) {
            Token* comma;
            func.symbols.insert({p->variable->symbol, p.value()});
            while ( (comma = peek(0)) != nullptr && comma->type == COMMA_TOKEN) {
                consume_token(COMMA_TOKEN, nullptr);
                p = parse_definition();
                func.symbols.insert({p->variable->symbol, p.value()});
            }
            consume_token(RPAREN_TOKEN, "expected a ')' symbol");
            consume_token(COLON_TOKEN, "expected a ':' symbol");
            func.ret_type = parse_type("expected a type");
            consume_token(LCURLY_TOKEN, "expected a '{' symbol");
            Token* mod;
            while((mod = peek(0)) != nullptr && mod->type == MOD_TOKEN) {
                std::optional<Label> label = parse_label();
                if (label.has_value())
                    func.labels.insert({ label->name->symbol, label.value() });
            }
            consume_token(RCURLY_TOKEN, "expected a '}' symbol");
            functions.insert({func.name->symbol, std::move(func)});

        }
    }

    std::optional<Label> Parser::parse_label() {
        Token* t;
        Label label;

        consume_token(MOD_TOKEN, "");
        label.name = peek(0);
        consume_token(IDENTIFIER_TOKEN, "expected an identifier");
        consume_token(COLON_TOKEN, "expected a ':' symbol");


        while ((t = peek(0)) != nullptr && t->type != MOD_TOKEN) {
            std::optional<Instruction> inst = parse_instruction();
            if (inst.has_value())
                label.instructions.push_back(std::move(inst.value()));
            else
                break;
        }

        return std::move(label);

    }

    Token* Parser::parse_ret() {
        consume_token(RET_TOKEN, "");
        return parse_literal("expected a value after 'ret'");
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
        else if (t->type == RET_TOKEN)
            return parse_ret();
        return {};
    }

    std::optional<Assignment> Parser::parse_assignment() {
        Token* t;
        Assignment inst;

        std::optional<Symbol> def = parse_definition(); 
        if (!def.has_value())
            return {};
        inst.identifier = def.value();
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

    std::optional<Symbol> Parser::parse_definition() {
        Symbol def;
        Token* t = consume_token(IDENTIFIER_TOKEN, "expected a register");
        if (t == nullptr)
            return {};
        def.variable = t;
        t = consume_token(COLON_TOKEN, "expected a ':' symbol");
        if (t == nullptr)
            return {};
        t = parse_type("expected a type");

        if (t == nullptr) {
            logger.log(def.variable->col, def.variable->row, "missing type");
            return {};
        }
        def.variable = t;

        return def;
        
    }

    std::optional<JmpInst> Parser::parse_jmp() {
        JmpInst inst;
        consume_token(GOTO_TOKEN, "");
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
        expr.left = parse_literal("expected an operand");
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
        
        expr.right = parse_literal("expected an operand");
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
        if (t == nullptr || t->type != type)
            throw std::logic_error(err_msg);
        
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
        throw std::logic_error(err_msg);
        return nullptr;
    }

    Token* Parser::get_next_token() {
        if (cur_token < tokens.size())
            return &tokens[cur_token];


        return nullptr;
    }

    Token* Parser::parse_binary_op(const char* err_msg) {
        return consume_any({
                PLUS_TOKEN,
                MINUS_TOKEN,
                DIV_TOKEN,
                MUL_TOKEN,
                RSHIFT_TOKEN,
                LSHIFT_TOKEN,
                NEQ_TOKEN,
                DEQ_TOKEN,
                LT_TOKEN,
                LTE_TOKEN,
                BT_TOKEN,
                BTE_TOKEN
                },
                err_msg);
    }

    Token* Parser::parse_type(const char* err_msg) {
        return  consume_any({
                I8_TYPE_TOKEN,
                I16_TYPE_TOKEN,
                I32_TYPE_TOKEN,
                I64_TYPE_TOKEN,
                U8_TYPE_TOKEN,
                U16_TYPE_TOKEN,
                U32_TYPE_TOKEN,
                U64_TYPE_TOKEN,
                F32_TYPE_TOKEN,
                F64_TYPE_TOKEN
                },
                err_msg);
    }
    Token* Parser::parse_literal(const char* err_msg) {
        return consume_any({IDENTIFIER_TOKEN, FLOAT_TOKEN, INTEGER_TOKEN}, err_msg);
    }

    void Parser::recover(std::initializer_list<TokenType> safe_tokens) {
        while (cur_token < tokens.size()) {
            Token& token = tokens[cur_token];
            for (TokenType t : safe_tokens) {
                if (token.type == t)
                    return;
            }
            cur_token++;
        }
    }

    const std::vector<std::unique_ptr<Error>>& Parser::get_errors() { return logger.get_errors(); }

    const std::unordered_map<std::string, Function>& Parser::get_functions() {
        return functions;
    }

}

