#include "../include/codegen.h"


namespace UASM {

    CodeGenerator::CodeGenerator(std::unique_ptr<Program> _program) : program(std::move(_program)) {};

    void CodeGenerator::generate(const char* file_path) {
        output_file.open(file_path);
        for (auto&[_,f]: program->functions)
            compile_function(f);
        
    }
    void CodeGenerator::compile_function(Function& func) {
        for (auto&[_, l]: func.labels)
            compile_label(l);
    }
    
    void CodeGenerator::compile_label(Label& label) {
        for (Instruction& inst : label.instructions)
            compile_instruction(inst);
    }

    void CodeGenerator::compile_instruction(Instruction& inst) {
        if (std::holds_alternative<Assignment>(inst))
                compile_assignment(std::get<Assignment>(inst));
        else if (std::holds_alternative<JmpInst>(inst))
            compile_goto(std::get<JmpInst>(inst));
    }


    x86_64Generator::x86_64Generator(std::unique_ptr<Program> _program) : CodeGenerator(std::move(_program)) {}

    void x86_64Generator::compile_binary_expr(BinaryExpr& expr) {

    }

    void x86_64Generator::compile_goto(JmpInst& inst) {}




}
