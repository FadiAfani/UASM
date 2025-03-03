#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <memory>
#include "parser.h"
#include "cfg.h"

namespace UASM {

    class Optimizer : public ProgramVisitor {

        public:
            virtual void optimize() = 0;
            virtual ~Optimizer() {}
    };
    
    class CFGOptimizer : Optimizer {
        protected:
            std::unique_ptr<CFGData> cd;
        public:
            CFGOptimizer(std::unique_ptr<CFGData> _cd);
            void optimize() override;
    };

}



#endif
