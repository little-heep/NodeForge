//
// Created by Micheal on 2026/4/28.
//

// You may need to build the project (run Qt uic code generator) to get "ui_PortItem.h" resolved

#include "PortItem.h"
#include "ConnectionItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

PortItem::PortItem(PortType type, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    m_type = type;
    // 可以根据类型设置不同的颜色或形状
    if (m_type == Input) {
        setToolTip("Input Port");
    } else {
        setToolTip("Output Port");
    }

    setAcceptedMouseButtons(Qt::LeftButton);
}

void PortItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (m_type == Output) {
        // 1. 创建临时连线
        m_tempConn = new ConnectionItem();
        m_tempConn->setStartPort(this);
        scene()->addItem(m_tempConn);
        event->accept();
        return;
    }
    event->ignore();
}

void PortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_tempConn) {
        // 2. 线条终点跟随鼠标位置 (scenePos)
        m_tempConn->setEndPoint(event->scenePos());
        m_tempConn->updatePath();
        event->accept();
        return;
    }
    event->ignore();
}

void PortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (m_tempConn) {
        // 3. 碰撞检测：看看松开位置有没有别的端口
        QGraphicsItem* item = scene()->itemAt(event->scenePos(), QTransform());
        PortItem* targetPort = dynamic_cast<PortItem*>(item);

        if (targetPort && targetPort->m_type == Input && targetPort->parentItem() != this->parentItem()) {
            // 连接成功：固化线条
            m_tempConn->setEndPort(targetPort);
            //m_tempConn->setEndPoint(QPointF()); // 清除临时点
            m_connections.append(m_tempConn);
            targetPort->m_connections.append(m_tempConn);
            m_tempConn->updatePath();
        } else {
            // 连接失败：删除临时线
            delete m_tempConn;
        }
        m_tempConn = nullptr;
        event->accept();
        return;
    }
    event->ignore();
}

void PortItem::updateConnections() {
    for (ConnectionItem *conn : m_connections) {
        if (conn) {
            conn->updatePath();
        }
    }

    if (m_tempConn) {
        m_tempConn->updatePath();
    }
}