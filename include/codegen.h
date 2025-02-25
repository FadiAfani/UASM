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
            virtual ~CodeGenerator() {}

    };




    class x86_64Generator {
        public:
            x86_64Generator(std::unique_ptr<Program> _program);


    };


}


#endif
