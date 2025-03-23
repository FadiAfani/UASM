#include "../include/parser.h"
#include <iostream>

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

    std::unique_ptr<Program> Parser::parse() {
        this->program = std::make_unique<Program>();
        Token* t;
        while((t = peek(0)) != nullptr) {
            parse_stmt();
        }

        return std::move(this->program);
    }

    void Parser::parse_stmt() {
        Token* t;
        if ((t = peek(0))->type == AT_TOKEN)
            parse_function();
        else if (t->type == EXTERN_TOKEN)
            parse_extern_stmt();

    }
    
    void Parser::parse_extern_stmt() {
        consume_token(EXTERN_TOKEN, "");
        auto id = consume_token(IDENTIFIER_TOKEN, "expected an identifier");
        if (id.has_value()) {
            program->externs[id->symbol] = &id.value();
        }
    }

    void Parser::parse_function() {
        Function func;
        std::optional<Token> t;
        consume_token(AT_TOKEN, "");
        t = consume_token(IDENTIFIER_TOKEN, "expected an identifier");
        if (!t.has_value())
            return;
        func.name = std::move(t.value());
        consume_token(LPAREN_TOKEN, "expected a '(' symbol");
        std::optional<Symbol> p = parse_definition();
        size_t order = 0;
        if (p.has_value()) {
            Token* comma;
            func.symbols.insert({p->variable.symbol, p.value()});
            while ( (comma = peek(0)) != nullptr && comma->type == COMMA_TOKEN) {
                consume_token(COMMA_TOKEN, nullptr);
                p = parse_definition();
                func.symbols.insert({p->variable.symbol, p.value()});
            }
            consume_token(RPAREN_TOKEN, "expected a ')' symbol");
            consume_token(COLON_TOKEN, "expected a ':' symbol");
            t = parse_type("expected a type");
            if (!t.has_value())
                return;
            consume_token(LCURLY_TOKEN, "expected a '{' symbol");
            Token* mod;
            while((mod = peek(0)) != nullptr && mod->type == MOD_TOKEN) {
                std::optional<Label> label = parse_label(func);
                if (label.has_value()) {
                    label.value().order = order++;
                    func.labels.insert({ label->name.symbol, label.value() });
                }
            }
            consume_token(RCURLY_TOKEN, "expected a '}' symbol");
            program->functions.insert({func.name.symbol, std::move(func)});

        }
    }

    std::optional<Label> Parser::parse_label(Function& func) {
        Token* t;
        Label label;
        std::optional<Token> ot;

        consume_token(MOD_TOKEN, "");
        ot = consume_token(IDENTIFIER_TOKEN, "expected an identifier");
        if (!ot.has_value())
            return std::nullopt;
        label.name = std::move(ot.value());
        consume_token(COLON_TOKEN, "expected a ':' symbol");

        while ((t = peek(0)) != nullptr && t->type != MOD_TOKEN) {
            std::optional<Instruction> inst = parse_instruction(func);
            if (inst.has_value())
                label.instructions.push_back(std::move(inst.value()));
            else
                break;
        }

        return std::move(label);

    }

    std::optional<Return> Parser::parse_ret() {
        consume_token(RET_TOKEN, "");
        auto t = parse_literal("expected a value after 'ret'");
        return t == std::nullopt ? std::nullopt : std::make_optional(Return(std::move(t.value())));
    }

    std::optional<Instruction> Parser::parse_instruction(Function& func) {
        Token* t = peek(0);
        std::optional<Instruction> inst;
        if (t == nullptr)
            return {};
        if (t->type == IDENTIFIER_TOKEN) 
            return parse_assignment(func);
        else if (t->type == GOTO_TOKEN)
            return parse_jmp();
        else if (t->type == RET_TOKEN)
            return parse_ret();
        return {};
    }

    std::optional<Assignment> Parser::parse_assignment(Function& func) {
        std::optional<Token> t;
        Assignment inst;

        std::optional<Symbol> def = parse_definition(); 
        if (!def.has_value())
            return {};
        inst.identifier = def.value();
        t = consume_token(EQ_TOKEN, "missing '=' symbol");
        if (!t.has_value())
            return {};
        auto expr = parse_expr();
        if (!expr.has_value()) {
            return {};
        }

        func.insert_symbol(def.value());
        inst.expr = expr.value(); 
        return inst;
        
    }

    std::optional<Symbol> Parser::parse_definition() {
        Symbol def;
        auto t = consume_token(IDENTIFIER_TOKEN, "expected a register");
        if (!t.has_value())
            return std::nullopt;
        def.variable = std::move(t.value());
        t = consume_token(COLON_TOKEN, "expected a ':' symbol");
        if (!t.has_value())
            return std::nullopt;
        t = parse_type("expected a type");

        if (!t.has_value()) {
            return std::nullopt;
        }

        def.type = std::move(t.value());

        return def;
        
    }

    std::optional<JmpInst> Parser::parse_jmp() {
        JmpInst inst;
        std::optional<Token> t;
        Token* id;
        consume_token(GOTO_TOKEN, "");
        t = consume_token(IDENTIFIER_TOKEN, "goto instructions require at least a target label");
        if (!t.has_value())
            return std::nullopt;
        inst.target = std::move(t.value()); 
        if ((id = peek(0)) != nullptr && id->type == IDENTIFIER_TOKEN) {
            inst.cond = std::move(*id);
            ++cur_token;
        } else {
            inst.cond = std::nullopt;

        }

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
            expr = *t1;
        }
        return expr;
    }

    std::optional<BinaryExpr> Parser::parse_binary_expr() {
        BinaryExpr expr;
        std::optional<Token> t;
        t = parse_literal("expected an operand");
        if (!t.has_value())
            return std::nullopt;
        expr.left = std::move(t.value());
        t = consume_any(
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

        if (!t.has_value())
            return std::nullopt;

        expr.op = std::move(t.value());
        
        t = parse_literal("expected an operand");


        if (!t.has_value())
            return std::nullopt;

        expr.right = std::move(t.value());

        return expr;
    };

    Token* Parser::peek(size_t n) {
        if (cur_token + n >= tokens.size())
            return nullptr;

        return &tokens[cur_token + n];

    }

    std::optional<Token> Parser::consume_token(TokenType type, const char* err_msg) {
        auto t = get_next_token();
        if (!t.has_value() || t->type != type) {
            throw std::logic_error(err_msg);
        }
        
        this->cur_token++;
        return std::move(t.value());
    }

    std::optional<Token> Parser::consume_any(std::initializer_list<TokenType> types, const char* err_msg) {
        Token* token; 
        for (TokenType t : types) {
            if ((token = peek(0)) && token->type == t) {
                cur_token++;
                return *token;
            }
            
        }
        throw std::logic_error(err_msg);
    }

    std::optional<Token> Parser::get_next_token() {
        if (cur_token < tokens.size())
            return tokens[cur_token];


        return std::nullopt;
    }

    std::optional<Token> Parser::parse_binary_op(const char* err_msg) {
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

    std::optional<Token> Parser::parse_type(const char* err_msg) {
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
    std::optional<Token> Parser::parse_literal(const char* err_msg) {
        std::optional<Token> t = consume_any({IDENTIFIER_TOKEN, FLOAT_TOKEN, INTEGER_TOKEN}, err_msg);
        return t;

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


}

