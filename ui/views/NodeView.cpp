//
// Created by Micheal on 2026/4/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_NodeView.h" resolved

#include "NodeView.h"
#include "NodeView.h"
#include "../items/NodeItem.h"
#include "../items/PortItem.h"
#include "../items/ConnectionItem.h"
#include "../../core/NodeGraph.h"

#include <QKeyEvent>
#include <QSet>


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

    // 1) 先收集要删除的节点
    QList<NodeItem*> nodeItems;
    for (QGraphicsItem* item : selected) {
        if (auto* nodeItem = dynamic_cast<NodeItem*>(item)) {
            nodeItems.append(nodeItem);
        }
    }

    // 2) 收集这些节点关联的所有连线，避免重复删除
    QSet<ConnectionItem*> connectionsToDelete;
    for (NodeItem* nodeItem : nodeItems) {
        for (QGraphicsItem* child : nodeItem->childItems()) {
            if (auto* port = dynamic_cast<PortItem*>(child)) {
                for (ConnectionItem* conn : port->connections()) {
                    connectionsToDelete.insert(conn);
                }
            }
        }
    }

    // 3) 删除连线，同时从端口列表中移除
    for (ConnectionItem* conn : connectionsToDelete) {
        if (!conn) continue;

        if (conn->startPort()) {
            conn->startPort()->removeConnection(conn);
        }
        if (conn->endPort()) {
            conn->endPort()->removeConnection(conn);
        }

        scene()->removeItem(conn);
        delete conn;
    }

    // 4) 删除节点，同时从 graph 中删除对应 model
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
