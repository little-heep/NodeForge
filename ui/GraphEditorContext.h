//
// Created by Micheal on 2026/6/10.
//

#ifndef NODEFORGE_GRAPHEDITORCONTEXT_H
#define NODEFORGE_GRAPHEDITORCONTEXT_H

#include <QObject>
#include <QHash>
#include <QJsonObject>
#include <QList>

class QUndoStack;
class QGraphicsItem;
class QWidget;

class NodeScene;
class NodeGraph;
class NodeModel;
class NodeItem;
class PortItem;

class GraphEditorContext : public QObject {
    Q_OBJECT
public:
    explicit GraphEditorContext(NodeScene* scene, NodeGraph* graph, QUndoStack* undoStack, QObject* parent = nullptr);

    void clearDocument();
    void rebuildFromJson(const QJsonObject& root);

    // 用户动作入口：这些方法内部负责 push 对应的 undo command
    void requestAddNode(const QJsonObject& nodeObj, const QPointF& pos);
    void requestDeleteSelection(const QList<QGraphicsItem*>& selection);
    void requestConnect(PortItem* outPort, PortItem* inPort);
    void requestMoveNode(NodeItem* item, const QPointF& oldPos, const QPointF& newPos);
    void requestEditNodeValue(NodeModel* model, QWidget* parent);

    NodeItem* findNodeItem(int nodeId) const;
    NodeModel* findNodeModel(int nodeId) const;

    // 给命令用的底层操作
    NodeItem* createNodeFromSnapshot(const QJsonObject& nodeObj, const QPointF& pos);
    bool removeNodeById(int nodeId);
    bool createConnection(int outId, int outIdx, int inId, int inIdx);
    bool removeConnection(int outId, int outIdx, int inId, int inIdx);
    bool moveNode(int nodeId, const QPointF& pos);
    bool applyModelState(int nodeId, const QJsonObject& modelState);

    QJsonObject snapshotNode(NodeModel* model, NodeItem* item) const;
    QJsonObject snapshotModel(NodeModel* model) const;

    QUndoStack* undoStack() const { return m_undoStack; }

private:
    NodeScene* m_scene = nullptr;
    NodeGraph* m_graph = nullptr;
    QUndoStack* m_undoStack = nullptr;

    QHash<int, NodeItem*> m_itemById;
};



#endif //NODEFORGE_GRAPHEDITORCONTEXT_H