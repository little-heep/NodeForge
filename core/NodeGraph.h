#ifndef NODEFORGE_NODEGRAPH_H
#define NODEFORGE_NODEGRAPH_H

#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <QDebug>
#include "../model/NodeModel.h"

struct Connection {
    NodeModel* outNode = nullptr;
    int outPortIdx = -1;
    NodeModel* inNode = nullptr;
    int inPortIdx = -1;
};

class NodeGraph {
public:
    NodeGraph() = default;
    ~NodeGraph() { clear(); }

    // graph takes ownership of 'node' (delete in clear)
    void addNode(NodeModel* node);

    // returns true if connection created; does not mutate node data immediately
    bool addConnection(NodeModel* out, int outIdx, NodeModel* in, int inIdx);

    // execute with topo sort + propagate outputs -> inputs
    void execute();

    // clear graph and owned nodes
    void clear();

    const std::vector<NodeModel*>& nodes() const { return m_nodes; }
    const std::vector<Connection>& connections() const { return m_conns; }

private:
    // Kahn algorithm — returns execution order (may be partial if cycles exist)
    std::vector<NodeModel*> topoSort();

private:
    std::vector<NodeModel*> m_nodes;      // owned pointers
    std::vector<Connection> m_conns;
};

#endif //NODEFORGE_NODEGRAPH_H
