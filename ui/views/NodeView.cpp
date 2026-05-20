//
// Created by Micheal on 2026/4/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_NodeView.h" resolved

#include "NodeView.h"
#include "../items/NodeItem.h"
#include "../items/PortItem.h"
#include "../items/ConnectionItem.h"
#include "../../core/NodeGraph.h"

#include <QKeyEvent>
#include <QSet>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QGraphicsScene>


NodeView::NodeView(QWidget *parent) : QGraphicsView(parent) {
    setRenderHint(QPainter::Antialiasing); // 抗锯齿
    setRenderHint(QPainter::TextAntialiasing);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(FullViewportUpdate);

    // 开启中键拖拽画布
    setDragMode(QGraphicsView::ScrollHandDrag);

    // 去掉滚动条（可选）
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 缩放锚点设为鼠标下
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setFocusPolicy(Qt::StrongFocus);
}

NodeView::~NodeView() {

}

void NodeView::wheelEvent(QWheelEvent *event) {
    // 计算缩放系数
    double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor); // 放大
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor); // 缩小
    }
}

void NodeView::keyPressEvent(QKeyEvent *event) {
    if (event->key() != Qt::Key_Delete || !scene()) {
        QGraphicsView::keyPressEvent(event);
        return;
    }

    const auto selected = scene()->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    // 1) 收集选中的节点 + 选中的连线
    QList<NodeItem*> nodeItems;
    QSet<ConnectionItem*> connectionsToDelete;

    for (QGraphicsItem* item : selected) {
        if (auto* nodeItem = dynamic_cast<NodeItem*>(item)) {
            nodeItems.append(nodeItem);
        } else if (auto* connItem = dynamic_cast<ConnectionItem*>(item)) {
            connectionsToDelete.insert(connItem);
        }
    }

    // 2) 把“选中节点”关联的所有连线也加入删除集合（去重）
    for (NodeItem* nodeItem : nodeItems) {
        if (!nodeItem) continue;
        for (QGraphicsItem* child : nodeItem->childItems()) {
            if (auto* port = dynamic_cast<PortItem*>(child)) {
                for (ConnectionItem* conn : port->connections()) {
                    connectionsToDelete.insert(conn);
                }
            }
        }
    }

    // 3) 先删连线（避免节点删除后端口悬空）
    // 3) 先删连线（避免节点删除后端口悬空）
    for (ConnectionItem* conn : connectionsToDelete) {
        if (!conn) continue;

        auto* sp = conn->startPort();
        auto* ep = conn->endPort();

        // 先从 graph 删除“数据连接”
        if (sp && ep) {
            NodeGraph* graph = nullptr;

            // 从端口父节点拿 graph（PortItem 没有 graph() getter）
            if (auto* n = dynamic_cast<NodeItem*>(sp->parentItem())) graph = n->graph();
            if (!graph) {
                if (auto* n = dynamic_cast<NodeItem*>(ep->parentItem())) graph = n->graph();
            }

            if (graph) {
                if (NodeModel* outM = sp->model(); NodeModel* inM = ep->model()) {
                    graph->removeConnection(outM, sp->index(), inM, ep->index());
                }
            }
        }

        // 再从端口的 UI 连接列表移除
        if (sp) sp->removeConnection(conn);
        if (ep) ep->removeConnection(conn);

        scene()->removeItem(conn);
        delete conn;
    }


    // 4) 再删节点，同时从 graph 删除对应 model
    for (NodeItem* nodeItem : nodeItems) {
        if (!nodeItem) continue;

        if (NodeGraph* graph = nodeItem->graph()) {
            graph->removeNode(nodeItem->model());
        }

        scene()->removeItem(nodeItem);
        delete nodeItem;
    }

    event->accept();
}

// helper to find NodeItem in scene
static NodeItem* asNodeItem(QGraphicsItem* it) {
    return dynamic_cast<NodeItem*>(it);
}

bool NodeView::saveToFile(const QString &path) {
    if (!scene()) return false;

    // 1) collect NodeItems -> map model* -> pos
    QHash<NodeModel*, NodeItem*> modelToItem;
    for (QGraphicsItem* it : scene()->items()) {
        if (auto *ni = asNodeItem(it)) {
            if (ni->model()) modelToItem[ni->model()] = ni;
        }
    }
    if (modelToItem.isEmpty()) {
        // allow saving empty graph
    }

    // 2) get graph pointer (assume all NodeItems share same graph)
    NodeGraph* graph = nullptr;
    if (!modelToItem.isEmpty()) {
        auto it = modelToItem.constBegin(); // QHash::constBegin() 返回迭代器
        if (it != modelToItem.constEnd()) {
            NodeItem* anyItem = it.value();
            if (anyItem) graph = anyItem->graph();
        }
    }

    // 作为后备，再遍历所有 values 找到第一个带 graph 的 NodeItem
    if (!graph) {
        for (NodeItem* ni : modelToItem) {
            if (ni && ni->graph()) { graph = ni->graph(); break; }
        }
    }

    QJsonObject root;
    root["version"] = 1;

    // nodes array
    QJsonArray nodesArr;
    for (auto it = modelToItem.constBegin(); it != modelToItem.constEnd(); ++it) {
        NodeModel* m = it.key();
        NodeItem* ni = it.value();
        if (!m) continue;

        QJsonObject no;
        no["id"] = m->id();
        no["type"] = m->typeName();
        no["caption"] = m->caption();

        QPointF pos = ni ? ni->pos() : QPointF(0,0);
        QJsonObject posObj;
        posObj["x"] = pos.x();
        posObj["y"] = pos.y();
        no["pos"] = posObj;

        // node-specific data via model->toJson()
        no["data"] = m->toJson();

        nodesArr.append(no);
    }
    root["nodes"] = nodesArr;

    // connections array: prefer graph->connections()
    QJsonArray connsArr;
    if (graph) {
        const auto &conns = graph->connections();
        for (const auto &c : conns) {
            if (!c.outNode || !c.inNode) continue;
            QJsonObject co;
            co["out"] = c.outNode->id();
            co["outIdx"] = c.outPortIdx;
            co["in"] = c.inNode->id();
            co["inIdx"] = c.inPortIdx;
            connsArr.append(co);
        }
    } else {
        // fallback: build connections from ConnectionItem in scene
        for (QGraphicsItem* it : scene()->items()) {
            auto *ci = dynamic_cast<ConnectionItem*>(it);
            if (!ci) continue;
            auto* sp = ci->startPort();
            auto* ep = ci->endPort();
            if (!sp || !ep) continue;
            NodeModel* outM = sp->model();
            NodeModel* inM = ep->model();
            if (!outM || !inM) continue;
            QJsonObject co;
            co["out"] = outM->id();
            co["outIdx"] = sp->index();
            co["in"] = inM->id();
            co["inIdx"] = ep->index();
            connsArr.append(co);
        }
    }
    root["connections"] = connsArr;

    // write file
    QJsonDocument doc(root);
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for write:" << path;
        return false;
    }
    f.write(doc.toJson(QJsonDocument::Indented));
    f.close();
    return true;
}