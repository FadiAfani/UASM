#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <memory>
#include "parser.h"
#include "program_visitor.h"

namespace UASM {

    class Optimizer : ProgramVisitor {
        protected:
            std::unique_ptr<Program> program;

        public:
            virtual void optimize() = 0;
            virtual ~Optimizer() {}
    };

}



#endif
