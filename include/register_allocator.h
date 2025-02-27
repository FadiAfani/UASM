#ifndef REGISTER_ALLOCATOR_H
#define REGISTER_ALLOCATOR_H

#include "optimizer.h"

namespace UASM {

    class RegisterAllocator : Optimizer {
        private:
            std::unordered_map<Token*, std::vector<unsigned int>> graph;
        public:
            void connect(Token* in, Token* out);



    };

}


#endif 
