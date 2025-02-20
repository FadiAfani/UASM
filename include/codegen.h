#ifndef CODEGEN_H
#define CODEGEN_H

#include <fstream>
#include "../include/parser.h"

namespace UASM {

    class InterferenceGraph {};

    class CodeGenerator {
        protected:
            std::unique_ptr<Program> program = nullptr;
            std::ofstream output_file;
        public:
            CodeGenerator(std::unique_ptr<Program> program);
            virtual void generate(const char* file_path);
            virtual void alloc_registers(std::vector<unsigned int> colors, InterferenceGraph& graph);
            virtual void compile_function(Function& func);
            virtual void compile_label(Label& label);
            virtual void compile_instruction(Instruction& inst);
            virtual void compile_binary_expr(BinaryExpr& expr) = 0;
            virtual void compile_assignment(Assignment& assignment) = 0;
            virtual void compile_goto(JmpInst& inst) = 0;
            virtual void compile_ret() = 0;
            virtual ~CodeGenerator() {}

    };




    class x86_64Generator : CodeGenerator {
        public:
            x86_64Generator(std::unique_ptr<Program> _program);
            void compile_binary_expr(BinaryExpr& expr) override;
            void compile_assignment(Assignment& assignment) override;
            void compile_goto(JmpInst& inst) override;


    };


}


#endif
