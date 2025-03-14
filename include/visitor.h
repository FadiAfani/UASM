#ifndef VISIOR_H
#define VISIOR_H

#include "tokenizer.h"
#include <list>
#include <variant>
#include <optional>
#include <unordered_set>
#include <functional>
#include <stack>
#include <queue>

namespace UASM {

    class ProgramVisitor;


    struct Literal {
        Token value;

        Literal(Token _value);
    };



    struct Symbol {
        Token variable;
        Token type;
        unsigned int reg;
    };

    struct BinaryExpr : Unit {
        Token left;
        Token op;
        Token right;

        void print() override;
        void accept(ProgramVisitor& visitor) override;
    };

    struct FuncCall : Unit {
        Token id;
        std::vector<std::reference_wrapper<Token>> args;

        void print() override;
        void accept(ProgramVisitor& visitor) override;
    };

    struct UnaryExpr : Unit {
        Token op;
        Token value;

        void print() override;
        void accept(ProgramVisitor& visitor) override;

    };

    typedef std::variant<BinaryExpr,Token> Expr;

    struct Assignment : Unit {
        Symbol identifier;
        Expr expr;

        void print() override;
        void accept(ProgramVisitor& visitor) override;

    };

    struct JmpInst : Unit {
        std::optional<Token> cond;
        Token target;

        void print() override;
        void accept(ProgramVisitor& visitor) override;
    };

    struct Return : Unit {
        Token value;

        Return(Token _value);

        void print() override;
        void accept(ProgramVisitor& visitor) override;
    };

    typedef std::variant<Assignment, JmpInst, Return, FuncCall> Instruction;


    struct Label : Unit {
        Token name;
        std::vector<Instruction> instructions;

        void print() override;
        void accept(ProgramVisitor& visitor) override;
    };

    struct Function : Unit {
        Token name;
        Token ret_type;
        std::unordered_map<std::string, Label> labels;
        std::unordered_map<std::string, Symbol> symbols;

        void print() override;
        void accept(ProgramVisitor& visitor) override;
        void insert_symbol(Symbol sym);
    };

    struct BasicBlock : Unit {
        Function& pf;
        size_t tag;
        std::list<Instruction> instructions;
        std::list<BasicBlock*> predecessors;
        std::list<BasicBlock*> successors;

        BasicBlock(Function& _pf);
        void print() override;
        void accept(ProgramVisitor& visitor) override;

        class PostOrder {
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = BasicBlock;
            using pointer = BasicBlock*;
            using reference = BasicBlock&;


            private:
                std::stack<pointer> dfs;

            public:
                PostOrder(pointer root) {
                    if (root) {
                        std::unordered_set<pointer> visited;
                        std::function<void(pointer, std::unordered_set<pointer>)> traverse = [&](pointer node, std::unordered_set<pointer> visited) {
                            dfs.push(node);
                            for (BasicBlock* bb : node->successors) {
                                if (visited.count(bb) == 0)
                                    traverse(bb, visited);
                            }

                        };
                        traverse(root, visited);
                    }
                }
                
                reference operator*() {
                    return *dfs.top();
                }
                pointer operator->() {
                    return dfs.top();
                }
                PostOrder& operator++() {
                    if (!dfs.empty())
                        dfs.pop();
                    return *this;
                }

                bool empty() { return dfs.size() > 0; }
        };
        class InOrder {

            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = BasicBlock;
            using pointer = BasicBlock*;
            using reference = BasicBlock&;

            private:
                std::queue<pointer> bfs;
                std::unordered_set<pointer> visited;

            public:
                InOrder(pointer root) {
                    bfs.push(root);
                }

                reference operator*() {
                    return *bfs.front();
                }

                pointer operator->() {
                    return bfs.front();
                }

                InOrder& operator++() {
                    pointer cur = bfs.front();
                    bfs.pop();

                    for (BasicBlock* bb : cur->successors) {
                        bfs.push(bb);
                    }
                    return *this;

                }

                bool empty() { return bfs.size() > 0; }


        };
    };

    struct Program : Unit {
        std::unordered_map<std::string, Function> functions;
        std::unordered_map<std::string, Token*> externs;

        void print() override;
        void accept(ProgramVisitor& visitor) override;

    };

    class InstructionVisitor {
        public:
            virtual void visit_binary_expr(BinaryExpr& expr) = 0;
            virtual void visit_literal(Token& literal) = 0;
            virtual void visit_assignment(Assignment& inst) = 0;
            virtual void visit_jmp(JmpInst& inst) = 0;
            virtual void visit_ret(Return& ret_val) = 0;

    };

    class ProgramVisitor : public InstructionVisitor {
        public:
            virtual void visit_instruction(Instruction& inst) {
                std::visit([this](auto&& i) {
                        i.accept(*this);
                }, inst);
            }
            virtual ~ProgramVisitor() {}
            virtual void visit_label(Label& label) = 0;
            virtual void visit_func(Function& func) = 0;
    };

}



#endif
