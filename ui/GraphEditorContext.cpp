// Created by Micheal on 2026/6/10.

#include "GraphEditorContext.h"
#include "./commands/GraphUndoCommands.h"

#include <QUndoStack>
#include <QList>
#include <QSet>
#include <QJsonArray>
#include <QGraphicsScene>

#include "../core/NodeGraph.h"
#include "../model/NodeFactory.h"
#include "../ui/views/NodeScene.h"
#include "../ui/items/NodeItem.h"
#include "../ui/items/PortItem.h"
#include "../ui/items/ConnectionItem.h"
#include "../model/NodeModel.h"

GraphEditorContext::GraphEditorContext(NodeScene* scene, NodeGraph* graph, QUndoStack* undoStack, QObject* parent)
    : QObject(parent), m_scene(scene), m_graph(graph), m_undoStack(undoStack)
{
    // nothing else for now
}

void GraphEditorContext::clearDocument()
{
    if (m_scene) m_scene->clear();
    if (m_graph) m_graph->clear();
    m_itemById.clear();
}

void GraphEditorContext::rebuildFromJson(const QJsonObject &root)
{
    clearDocument();

    if (!root.contains("nodes") || !root["nodes"].isArray()) return;
    QJsonArray nodesArr = root["nodes"].toArray();

    // 1) create all nodes
    for (const QJsonValue &v : nodesArr) {
        if (!v.isObject()) continue;
        QJsonObject no = v.toObject();

        // NodeFactory expects an object with "type" and optional "data"
        NodeModel* model = NodeFactory::createNodeFromJson(no);
        if (!model) continue;

        // add to graph (NodeGraph::addNode will assign id if needed)
        if (m_graph) m_graph->addNode(model);

        // create UI node item
        int inCount = static_cast<int>(model->inputs.size());
        int outCount = static_cast<int>(model->outputs.size());
        NodeItem* ni = new NodeItem(model->caption(), inCount, outCount, model, m_graph, this);
        // position
        if (no.contains("pos") && no["pos"].isObject()) {
            QJsonObject pos = no["pos"].toObject();
            double x = pos.contains("x") ? pos["x"].toDouble() : 0;
            double y = pos.contains("y") ? pos["y"].toDouble() : 0;
            ni->setPos(x, y);
        } else {
            ni->setPos(0,0);
        }
        if (m_scene) m_scene->addItem(ni);
        m_itemById[model->id()] = ni;
    }

    // 2) rebuild connections
    if (root.contains("connections") && root["connections"].isArray()) {
        QJsonArray connsArr = root["connections"].toArray();
        for (const QJsonValue &cv : connsArr) {
            if (!cv.isObject()) continue;
            QJsonObject co = cv.toObject();
            int outId = co["out"].toInt(-1);
            int outIdx = co["outIdx"].toInt(0);
            int inId = co["in"].toInt(-1);
            int inIdx = co["inIdx"].toInt(0);

            NodeItem* outItem = m_itemById.value(outId, nullptr);
            NodeItem* inItem  = m_itemById.value(inId, nullptr);
            if (!outItem || !inItem) continue;

            // find ports
            auto findPort = [](NodeItem* nodeItem, PortItem::PortType type, int idx)->PortItem* {
                for (QGraphicsItem* child : nodeItem->childItems()) {
                    if (auto p = dynamic_cast<PortItem*>(child)) {
                        if (p->portType() == type && p->index() == idx) return p;
                    }
                }
                return nullptr;
            };

            PortItem* sp = findPort(outItem, PortItem::Output, outIdx);
            PortItem* ep = findPort(inItem, PortItem::Input, inIdx);
            if (!sp || !ep) continue;

            // data-layer
            if (m_graph) m_graph->addConnection(sp->model(), sp->index(), ep->model(), ep->index());

            // ui-layer
            ConnectionItem* conn = new ConnectionItem();
            conn->setStartPort(sp);
            conn->setEndPort(ep);
            sp->addConnection(conn);
            ep->addConnection(conn);
            if (m_scene) m_scene->addItem(conn);
            conn->updatePath();
        }
    }

    // execute and refresh UI
    if (m_graph) m_graph->execute();
    if (m_scene) {
        for (QGraphicsItem* item : m_scene->items()) {
            if (auto nodeItem = dynamic_cast<NodeItem*>(item)) nodeItem->update();
        }
    }
}

void GraphEditorContext::requestAddNode(const QJsonObject &nodeObj, const QPointF &pos)
{
    if (!m_undoStack) return;
    m_undoStack->push(new AddNodeCommand(this, nodeObj, pos));
}

void GraphEditorContext::requestDeleteSelection(const QList<QGraphicsItem *> &selection)
{
    if (!m_undoStack) return;
    m_undoStack->push(new DeleteSelectionCommand(this, selection));
}

void GraphEditorContext::requestConnect(PortItem *outPort, PortItem *inPort)
{
    if (!m_undoStack || !outPort || !inPort) return;
    NodeModel* outM = outPort->model();
    NodeModel* inM = inPort->model();
    if (!outM || !inM) return;
    m_undoStack->push(new ConnectCommand(this, outM->id(), outPort->index(), inM->id(), inPort->index()));
}

void GraphEditorContext::requestMoveNode(NodeItem *item, const QPointF &oldPos, const QPointF &newPos)
{
    if (!m_undoStack || !item || !item->model()) return;
    int id = item->model()->id();
    if (oldPos == newPos) return;
    m_undoStack->push(new MoveNodeCommand(this, id, oldPos, newPos));
}

void GraphEditorContext::requestEditNodeValue(NodeModel *model, QWidget *parent)
{
    if (!m_undoStack || !model) return;
    QJsonObject before = snapshotModel(model);
    // this will present dialog and modify model in-place
    if (!model->editValue(parent)) return;
    QJsonObject after = snapshotModel(model);
    if (before != after) {
        m_undoStack->push(new EditNodeValueCommand(this, model->id(), before, after));
    }
}

NodeItem* GraphEditorContext::findNodeItem(int nodeId) const
{
    return m_itemById.value(nodeId, nullptr);
}

NodeModel* GraphEditorContext::findNodeModel(int nodeId) const
{
    if (auto ni = findNodeItem(nodeId)) return ni->model();
    return nullptr;
}

NodeItem* GraphEditorContext::createNodeFromSnapshot(const QJsonObject &nodeObj, const QPointF &pos)
{
    // Use NodeFactory to create model
    NodeModel* model = NodeFactory::createNodeFromJson(nodeObj);
    if (!model) return nullptr;

    if (m_graph) m_graph->addNode(model);

    int inCount = static_cast<int>(model->inputs.size());
    int outCount = static_cast<int>(model->outputs.size());
    NodeItem* ni = new NodeItem(model->caption(), inCount, outCount, model, m_graph,this);
    ni->setPos(pos);
    if (m_scene) m_scene->addItem(ni);
    m_itemById[model->id()] = ni;
    return ni;
}

bool GraphEditorContext::removeNodeById(int nodeId)
{
    NodeItem* ni = m_itemById.value(nodeId, nullptr);
    if (!ni) return false;

    // collect connections to remove
    QList<ConnectionItem*> connsToRemove;
    QSet<ConnectionItem*> seen;

    for (QGraphicsItem* child : ni->childItems()) {
        if (auto p = dynamic_cast<PortItem*>(child)) {
            for (ConnectionItem* c : p->connections()) {
                if (c && !seen.contains(c)) {
                    seen.insert(c);
                    connsToRemove.append(c);
                }
            }
        }
    }

    // remove connections (ui + data)
    for (ConnectionItem* conn : connsToRemove) {
        if (!conn) continue;
        PortItem* sp = conn->startPort();
        PortItem* ep = conn->endPort();
        if (sp && ep) {
            NodeModel* outM = sp->model();
            NodeModel* inM  = ep->model();
            if (m_graph && outM && inM) {
                m_graph->removeConnection(outM, sp->index(), inM, ep->index());
            }
        }
        if (sp) sp->removeConnection(conn);
        if (ep) ep->removeConnection(conn);
        if (m_scene) m_scene->removeItem(conn);
        delete conn;
    }

    // remove node UI and model (NodeGraph::removeNode deletes model)
    if (m_scene) m_scene->removeItem(ni);
    NodeModel* model = ni->model();
    delete ni; // NodeGraph::removeNode will delete model pointer; but NodeItem deletion does not delete model
    // remove from graph (this will delete model)
    if (m_graph && model) {
        m_graph->removeNode(model);
    }
    m_itemById.remove(nodeId);
    return true;
}

bool GraphEditorContext::createConnection(int outId, int outIdx, int inId, int inIdx)
{
    NodeItem* outItem = findNodeItem(outId);
    NodeItem* inItem  = findNodeItem(inId);
    if (!outItem || !inItem) return false;

    PortItem* sp = nullptr;
    PortItem* ep = nullptr;
    for (QGraphicsItem* child : outItem->childItems()) {
        if (auto p = dynamic_cast<PortItem*>(child)) {
            if (p->portType() == PortItem::Output && p->index() == outIdx) { sp = p; break; }
        }
    }
    for (QGraphicsItem* child : inItem->childItems()) {
        if (auto p = dynamic_cast<PortItem*>(child)) {
            if (p->portType() == PortItem::Input && p->index() == inIdx) { ep = p; break; }
        }
    }
    if (!sp || !ep) return false;

    if (m_graph && !m_graph->addConnection(sp->model(), sp->index(), ep->model(), ep->index())) {
        return false;
    }
    ConnectionItem* conn = new ConnectionItem();
    conn->setStartPort(sp);
    conn->setEndPort(ep);
    sp->addConnection(conn);
    ep->addConnection(conn);
    if (m_scene) m_scene->addItem(conn);
    conn->updatePath();
    return true;
}

bool GraphEditorContext::removeConnection(int outId, int outIdx, int inId, int inIdx)
{
    // search for a ConnectionItem in the scene matching endpoints
    if (!m_scene) return false;
    for (QGraphicsItem* it : m_scene->items()) {
        auto ci = dynamic_cast<ConnectionItem*>(it);
        if (!ci) continue;
        PortItem* sp = ci->startPort();
        PortItem* ep = ci->endPort();
        if (!sp || !ep) continue;
        NodeModel* outM = sp->model();
        NodeModel* inM  = ep->model();
        if (!outM || !inM) continue;
        if (outM->id() == outId && sp->index() == outIdx && inM->id() == inId && ep->index() == inIdx) {
            // remove from graph
            if (m_graph) m_graph->removeConnection(outM, outIdx, inM, inIdx);
            sp->removeConnection(ci);
            ep->removeConnection(ci);
            m_scene->removeItem(ci);
            delete ci;
            return true;
        }
    }
    return false;
}

bool GraphEditorContext::moveNode(int nodeId, const QPointF &pos)
{
    NodeItem* ni = findNodeItem(nodeId);
    if (!ni) return false;
    ni->setPos(pos);
    // itemChange in NodeItem will update connection graphics
    return true;
}

bool GraphEditorContext::applyModelState(int nodeId, const QJsonObject &modelState)
{
    NodeItem* ni = findNodeItem(nodeId);
    if (!ni) return false;
    NodeModel* model = ni->model();
    if (!model) return false;
    model->fromJson(modelState);
    // refresh UI
    ni->update();
    return true;
}

QJsonObject GraphEditorContext::snapshotNode(NodeModel *model, NodeItem *item) const
{
    QJsonObject no;
    if (!model) return no;
    no["id"] = model->id();
    no["type"] = model->typeName();
    no["caption"] = model->caption();
    // position
    QPointF p(0,0);
    if (item) p = item->pos();
    QJsonObject posObj;
    posObj["x"] = p.x();
    posObj["y"] = p.y();
    no["pos"] = posObj;
    // model data
    no["data"] = model->toJson();
    return no;
}

QJsonObject GraphEditorContext::snapshotModel(NodeModel *model) const
{
    if (!model) return QJsonObject();
    return model->toJson();
}
