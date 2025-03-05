#include "../include/optimizer.h"
#include <queue>
#include <set>

namespace UASM {
    CFGOptimizer::CFGOptimizer(CFGData* _cd) : cd(_cd) {}
    void CFGOptimizer::optimize() {
        for (auto&[_, bbs] : cd->cfgs) {
            std::queue<BasicBlock*> q;
            std::set<BasicBlock*> visited;
            for (auto& bb : bbs) {
                q.push(bb.get());
            }
            while (!q.empty()) {
                BasicBlock* cur = q.front();
                visited.insert(cur);
                q.pop();
                this->ctx = cur;
                cur->accept(*this);
                for (BasicBlock* bb : cur->successors) {
                    if (visited.count(bb) == 0)
                        q.push(bb);
                }
            }
        }
    }
}
