//
// Created by Micheal on 2026/5/1.
//

#ifndef NODEFORGE_NODEGRAPH_H
#define NODEFORGE_NODEGRAPH_H
#include <vector>
#include "../model/NodeModel.h"

struct Connection {
    NodeModel* outNode;
    int outPortIdx;
    NodeModel* inNode;
    int inPortIdx;
};

class NodeGraph {
public:
    void addConnection(NodeModel* out, int outIdx, NodeModel* in, int inIdx) {
        m_conns.push_back({out, outIdx, in, inIdx});
    }

    void execute() {
        // 1. 简单的顺序：先同步数据
        for (auto& c : m_conns) {
            c.inNode->inputs[c.inPortIdx] = c.outNode->outputs[c.outPortIdx];
        }
        // 2. 调用计算（实际开发中应使用拓扑排序确定顺序）
        for (auto node : m_nodes) {
            node->compute();
        }
    }

    std::vector<NodeModel*> m_nodes;
    std::vector<Connection> m_conns;
};


#endif //NODEFORGE_NODEGRAPH_H