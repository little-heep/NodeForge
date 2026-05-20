#include "NodeGraph.h"

void NodeGraph::addNode(NodeModel* node) {
    if (!node) return;
    // avoid duplicates
    if (std::find(m_nodes.begin(), m_nodes.end(), node) == m_nodes.end()) {
        m_nodes.push_back(node);
    }
}

bool NodeGraph::addConnection(NodeModel* out, int outIdx, NodeModel* in, int inIdx) {
    if (!out || !in) return false;
    if (outIdx < 0 || outIdx >= (int)out->outputs.size()) return false;
    if (inIdx < 0 || inIdx >= (int)in->inputs.size()) return false;

    // 检查是否已有相同连接
    for (const auto &c : m_conns) {
        if (c.outNode == out && c.outPortIdx == outIdx && c.inNode == in && c.inPortIdx == inIdx)
            return false;
    }

    m_conns.push_back({out, outIdx, in, inIdx});
    return true;
}

std::vector<NodeModel*> NodeGraph::topoSort() {
    std::unordered_map<NodeModel*, int> indeg;
    std::unordered_map<NodeModel*, std::vector<Connection>> outAdj;

    for (auto n : m_nodes) indeg[n] = 0;
    for (const auto &c : m_conns) {
        if (!c.inNode || !c.outNode) continue;
        indeg[c.inNode]++; // inNode 的入度增加
        outAdj[c.outNode].push_back(c);
    }

    std::queue<NodeModel*> q;
    for (auto &p : indeg) {
        if (p.second == 0) q.push(p.first);
    }

    std::vector<NodeModel*> order;
    while (!q.empty()) {
        NodeModel* cur = q.front(); q.pop();
        order.push_back(cur);

        auto it = outAdj.find(cur);
        if (it == outAdj.end()) continue;
        for (const auto &c : it->second) {
            indeg[c.inNode]--;
            if (indeg[c.inNode] == 0) q.push(c.inNode);
        }
    }

    if (order.size() < m_nodes.size()) {
        qDebug() << "NodeGraph::topoSort - graph has cycles or unreachable nodes. Partial order returned.";
        // 为完整性：把没有在 order 中出现的节点追加到后面（保证每个节点至少被返回一次）
        for (auto n : m_nodes) {
            if (std::find(order.begin(), order.end(), n) == order.end()) order.push_back(n);
        }
    }

    return order;
}

void NodeGraph::execute() {
    qDebug() << "=== NodeGraph execute start ===";
    qDebug() << "Nodes in graph:";
    for (auto n : m_nodes) {
        qDebug() << "  node ptr" << (void*)n << " caption=" << n->caption();
    }
    qDebug() << "Connections:";
    for (const auto &c : m_conns) {
        qDebug() << "  out=" << (void*)c.outNode << c.outNode->caption()
                 << "outIdx=" << c.outPortIdx
                 << " -> in=" << (void*)c.inNode << c.inNode->caption()
                 << " inIdx=" << c.inPortIdx;
    }
    // 按拓扑顺序执行并传播
    auto order = topoSort();
    qDebug() << "Topo order:";
    for (auto n : order) qDebug() << "  " << (void*)n << n->caption();
    // 对每个节点按顺序 compute，然后传播它的 outputs 到后继节点的 inputs
    for (NodeModel* node : order) {
        if (!node) continue;
        qDebug() << "Before compute:" << node->caption()
             << " inputs:" ;
        node->compute();
        qDebug() << "After compute:" << node->caption()
                 << " outputs:" ;


        for (const auto &c : m_conns) {
            if (c.outNode == node && c.outNode && c.inNode) {
                if (c.outPortIdx >= 0 && c.outPortIdx < (int)c.outNode->outputs.size() &&
                    c.inPortIdx >= 0 && c.inPortIdx < (int)c.inNode->inputs.size()) {
                    c.inNode->inputs[c.inPortIdx] = c.outNode->outputs[c.outPortIdx];
                }
            }
        }
    }
}

void NodeGraph::clear() {
    // delete owned nodes
    for (auto n : m_nodes) {
        delete n;
    }
    m_nodes.clear();
    m_conns.clear();
}

bool NodeGraph::removeNode(NodeModel* node) {
    if (!node) return false;

    // 先删除所有关联连接
    m_conns.erase(
        std::remove_if(m_conns.begin(), m_conns.end(),
                       [node](const Connection& c) {
                           return c.outNode == node || c.inNode == node;
                       }),
        m_conns.end()
    );

    // 再删除节点本身
    auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end()) {
        delete *it;
        m_nodes.erase(it);
        return true;
    }

    return false;
}
