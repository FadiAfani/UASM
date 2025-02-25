#include "../include/codegen.h"


namespace UASM {

    CodeGenerator::CodeGenerator(std::unique_ptr<Program> _program) : program(std::move(_program)) {};


}
