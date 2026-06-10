#include "GraphUndoCommands.h"
#include "../GraphEditorContext.h"
#include "../items/NodeItem.h"
#include "../items/PortItem.h"
#include "../items/ConnectionItem.h"

#include <QGraphicsItem>
#include <QSet>
#include <QString>

//
// AddNodeCommand
//
AddNodeCommand::AddNodeCommand(GraphEditorContext* ctx, const QJsonObject &nodeObj, const QPointF &pos, QUndoCommand *parent)
    : QUndoCommand(parent), m_ctx(ctx), m_nodeObj(nodeObj), m_pos(pos)
{
    setText("Add Node");
}

void AddNodeCommand::redo()
{
    if (!m_ctx) return;
    if (!m_created) {
        // first time: create and capture id
        // createNodeFromSnapshot will create model+item and register id
        QJsonObject snapshot = m_nodeObj;
        NodeItem* ni = m_ctx->createNodeFromSnapshot(snapshot, m_pos);
        if (ni && ni->model()) {
            m_nodeId = ni->model()->id();
            // ensure stored object contains id for future redo
            m_nodeObj["id"] = m_nodeId;
            m_created = true;
        }
    } else {
        // recreate node using saved snapshot (with id)
        m_ctx->createNodeFromSnapshot(m_nodeObj, m_pos);
    }
}

void AddNodeCommand::undo()
{
    if (!m_ctx) return;
    if (m_nodeId != 0) {
        m_ctx->removeNodeById(m_nodeId);
    }
}

//
// DeleteSelectionCommand
//
DeleteSelectionCommand::DeleteSelectionCommand(GraphEditorContext* ctx, const QList<QGraphicsItem *> &selection, QUndoCommand *parent)
    : QUndoCommand(parent), m_ctx(ctx)
{
    setText("Delete Selection");

    // create snapshots
    QSet<int> nodeIds;
    // first collect node snapshots
    for (QGraphicsItem* it : selection) {
        if (!it) continue;
        auto ni = dynamic_cast<NodeItem*>(it);
        if (ni && ni->model()) {
            NodeSnapshot ns;
            ns.nodeObj = m_ctx->snapshotNode(ni->model(), ni);
            ns.pos = ni->pos();
            m_nodes.append(ns);
            nodeIds.insert(ni->model()->id());
        }
    }
    // collect connection snapshots (unique)
    QSet<QString> seenConn;
    for (QGraphicsItem* it : selection) {
        auto ci = dynamic_cast<ConnectionItem*>(it);
        if (!ci) continue;
        auto sp = ci->startPort();
        auto ep = ci->endPort();
        if (!sp || !ep) continue;
        int outId = sp->model()->id();
        int inId  = ep->model()->id();
        int outIdx = sp->index();
        int inIdx  = ep->index();
        // store only if endpoints present
        QString key = QString("%1:%2->%3:%4").arg(outId).arg(outIdx).arg(inId).arg(inIdx);
        if (seenConn.contains(key)) continue;
        seenConn.insert(key);
        ConnectionSnapshot cs{outId,outIdx,inId,inIdx};
        m_connections.append(cs);
    }
    // also include any connections attached to selected nodes that were not explicitly selected
    // (to ensure node deletion cleans attached connections)
    for (const NodeSnapshot &ns : m_nodes) {
        int nid = ns.nodeObj["id"].toInt();
        NodeItem* ni = m_ctx->findNodeItem(nid);
        if (!ni) continue;
        for (QGraphicsItem* child : ni->childItems()) {
            if (auto p = dynamic_cast<PortItem*>(child)) {
                for (ConnectionItem* c : p->connections()) {
                    if (!c) continue;
                    PortItem* sp = c->startPort();
                    PortItem* ep = c->endPort();
                    if (!sp || !ep) continue;
                    int outId = sp->model()->id();
                    int inId  = ep->model()->id();
                    int outIdx = sp->index();
                    int inIdx  = ep->index();
                    QString key = QString("%1:%2->%3:%4").arg(outId).arg(outIdx).arg(inId).arg(inIdx);
                    if (seenConn.contains(key)) continue;
                    seenConn.insert(key);
                    ConnectionSnapshot cs{outId,outIdx,inId,inIdx};
                    m_connections.append(cs);
                }
            }
        }
    }
}

void DeleteSelectionCommand::redo()
{
    if (!m_ctx) return;
    // On first redo we delete nodes + connections captured
    // Delete connections first
    for (const ConnectionSnapshot &cs : m_connections) {
        m_ctx->removeConnection(cs.outId, cs.outIdx, cs.inId, cs.inIdx);
    }
    // Delete nodes
    for (const NodeSnapshot &ns : m_nodes) {
        int nid = ns.nodeObj["id"].toInt();
        m_ctx->removeNodeById(nid);
    }
    m_firstRedo = false;
}

void DeleteSelectionCommand::undo()
{
    if (!m_ctx) return;
    // recreate nodes (in original order)
    for (const NodeSnapshot &ns : m_nodes) {
        m_ctx->createNodeFromSnapshot(ns.nodeObj, ns.pos);
    }
    // recreate connections
    for (const ConnectionSnapshot&cs : m_connections) {
        m_ctx->createConnection(cs.outId, cs.outIdx, cs.inId, cs.inIdx);
    }
}

//
// ConnectCommand
//
ConnectCommand::ConnectCommand(GraphEditorContext* ctx, int outId, int outIdx, int inId, int inIdx, QUndoCommand* parent)
    : QUndoCommand(parent), m_ctx(ctx), m_outId(outId), m_outIdx(outIdx), m_inId(inId), m_inIdx(inIdx)
{
    setText("Connect");
}

void ConnectCommand::redo()
{
    if (!m_ctx) return;
    if (!m_done) {
        m_ctx->createConnection(m_outId, m_outIdx, m_inId, m_inIdx);
        m_done = true;
    } else {
        // redo after undo: re-create
        m_ctx->createConnection(m_outId, m_outIdx, m_inId, m_inIdx);
    }
}

void ConnectCommand::undo()
{
    if (!m_ctx) return;
    m_ctx->removeConnection(m_outId, m_outIdx, m_inId, m_inIdx);
    m_done = false;
}

//
// MoveNodeCommand
//
MoveNodeCommand::MoveNodeCommand(GraphEditorContext* ctx, int nodeId, const QPointF& oldPos, const QPointF& newPos, QUndoCommand* parent)
    : QUndoCommand(parent), m_ctx(ctx), m_nodeId(nodeId), m_oldPos(oldPos), m_newPos(newPos)
{
    setText("Move Node");
}

void MoveNodeCommand::redo()
{
    if (!m_ctx) return;
    m_ctx->moveNode(m_nodeId, m_newPos);
}

void MoveNodeCommand::undo()
{
    if (!m_ctx) return;
    m_ctx->moveNode(m_nodeId, m_oldPos);
}

//
// EditNodeValueCommand
//
EditNodeValueCommand::EditNodeValueCommand(GraphEditorContext* ctx, int nodeId, const QJsonObject &beforeState, const QJsonObject &afterState, QUndoCommand *parent)
    : QUndoCommand(parent), m_ctx(ctx), m_nodeId(nodeId), m_before(beforeState), m_after(afterState), m_done(false)
{
    setText("Edit Node Value");
}

void EditNodeValueCommand::redo()
{
    if (!m_ctx) return;
    m_ctx->applyModelState(m_nodeId, m_after);
    m_done = true;
}

void EditNodeValueCommand::undo()
{
    if (!m_ctx) return;
    m_ctx->applyModelState(m_nodeId, m_before);
    m_done = false;
}
