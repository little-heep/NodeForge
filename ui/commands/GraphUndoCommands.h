//
// Created by Micheal on 2026/6/10.
//

#ifndef NODEFORGE_GRAPHUNDOCOMMANDS_H
#define NODEFORGE_GRAPHUNDOCOMMANDS_H

#include <QUndoCommand>
#include <QJsonObject>
#include <QPointF>
#include <QList>

class GraphEditorContext;
class NodeItem;
class NodeModel;
class PortItem;
class QGraphicsItem;

class AddNodeCommand : public QUndoCommand {
public:
    AddNodeCommand(GraphEditorContext* ctx, const QJsonObject& nodeObj, const QPointF& pos, QUndoCommand* parent = nullptr);
    void undo() override;
    void redo() override;
private:
    GraphEditorContext* m_ctx;
    QJsonObject m_nodeObj;
    QPointF m_pos;
    int m_nodeId = 0;
    bool m_created = false;
};

class DeleteSelectionCommand : public QUndoCommand {
public:
    DeleteSelectionCommand(GraphEditorContext* ctx, const QList<QGraphicsItem*>& selection, QUndoCommand* parent = nullptr);
    void undo() override;
    void redo() override;
private:
    GraphEditorContext* m_ctx;

    struct NodeSnapshot {
        QJsonObject nodeObj;
        QPointF pos;
    };
    struct ConnectionSnapshot {
        int outId;
        int outIdx;
        int inId;
        int inIdx;
    };

    QList<NodeSnapshot> m_nodes;
    QList<ConnectionSnapshot> m_connections;
    bool m_firstRedo = true;
};

class ConnectCommand : public QUndoCommand {
public:
    ConnectCommand(GraphEditorContext* ctx, int outId, int outIdx, int inId, int inIdx, QUndoCommand* parent = nullptr);
    void undo() override;
    void redo() override;
private:
    GraphEditorContext* m_ctx;
    int m_outId;
    int m_outIdx;
    int m_inId;
    int m_inIdx;
    bool m_done = false;
};

class MoveNodeCommand : public QUndoCommand {
public:
    MoveNodeCommand(GraphEditorContext* ctx, int nodeId, const QPointF& oldPos, const QPointF& newPos, QUndoCommand* parent = nullptr);
    void undo() override;
    void redo() override;
private:
    GraphEditorContext* m_ctx;
    int m_nodeId;
    QPointF m_oldPos;
    QPointF m_newPos;
};

class EditNodeValueCommand : public QUndoCommand {
public:
    EditNodeValueCommand(GraphEditorContext* ctx, int nodeId, const QJsonObject& beforeState, const QJsonObject& afterState, QUndoCommand* parent = nullptr);
    void undo() override;
    void redo() override;
private:
    GraphEditorContext* m_ctx;
    int m_nodeId;
    QJsonObject m_before;
    QJsonObject m_after;
    bool m_done = false;
};


#endif //NODEFORGE_GRAPHUNDOCOMMANDS_H