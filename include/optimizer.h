#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <memory>
#include "parser.h"
#include "cfg.h"

namespace UASM {

    class Optimizer : ProgramVisitor {
        protected:
            std::unique_ptr<CFGData> cd;

        public:
            virtual void optimize() = 0;
            virtual ~Optimizer() {}
    };

}



#endif
