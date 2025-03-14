#include "../include/optimizer.h"

namespace UASM {
    CFGOptimizer::CFGOptimizer(CFGData* _cd) : cd(_cd) {}
    void CFGOptimizer::optimize() {
        for (auto &[_, bbs] : cd->cfgs) {
            if (bbs.size() > 0)
                bbs.front()->accept(*this);
        }
            
    }
}
