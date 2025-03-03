#include "../include/optimizer.h"

namespace UASM {
    CFGOptimizer::CFGOptimizer(std::unique_ptr<CFGData> _cd) : cd(std::move(_cd)) {}
    void CFGOptimizer::optimize() {
        for (auto&[_, bbs] : cd->cfgs) {
            for (auto& bb : bbs)
                bb->accept(*this);
        }
    }
}
