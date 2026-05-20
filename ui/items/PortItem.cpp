//
// Created by Micheal on 2026/4/28.
//

// You may need to build the project (run Qt uic code generator) to get "ui_PortItem.h" resolved

#include "PortItem.h"
#include "ConnectionItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include "../../model/NodeModel.h"
#include "../../core/NodeGraph.h"
#include <QStyle>
#include <QStyleOptionGraphicsItem>

PortItem::PortItem(PortType type, NodeModel* model, NodeGraph* graph,int index,QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    m_type = type;
    m_model = model;
    m_index = index;
    m_graph = graph;
    // 可以根据类型设置不同的颜色或形状
    if (m_type == Input) {
        setToolTip("Input Port");
    } else {
        setToolTip("Output Port");
    }

    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
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
        QList<QGraphicsItem*> items = scene()->items(event->scenePos());
        PortItem* targetPort = nullptr;
        for (QGraphicsItem* it : items) {
            if (it == this) continue;
            auto p = dynamic_cast<PortItem*>(it);
            if (p) { targetPort = p; break; }
        }

        if (targetPort && targetPort->m_type == Input && targetPort->parentItem() != this->parentItem()) {
            // 连接成功：固化线条
            m_tempConn->setEndPort(targetPort);
            //m_tempConn->setEndPoint(QPointF()); // 清除临时点
            m_connections.append(m_tempConn);
            targetPort->m_connections.append(m_tempConn);
            m_tempConn->updatePath();
            if (m_graph) {
                m_graph->addConnection(m_model, m_index, targetPort->m_model, targetPort->m_index);
            }
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

void PortItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {
    Q_UNUSED(option);
    painter->setRenderHint(QPainter::Antialiasing, true);

    QColor fillColor = (m_type == Input) ? QColor("#3b82f6") : QColor("#06b6d4"); // blue / cyan
    QColor border = QColor("#9aa8b6");

    // If hovered, draw slightly larger and brighter
    bool hovered = (option && (option->state & QStyle::State_MouseOver));
    qreal scale = hovered ? 1.3 : 1.0;
    QRectF r = boundingRect();
    QPointF center = r.center();
    qreal w = r.width() * scale;
    qreal h = r.height() * scale;
    QRectF drawRect(center.x() - w/2, center.y() - h/2, w, h);

    painter->setPen(QPen(border, 1));
    painter->setBrush(fillColor);
    painter->drawEllipse(drawRect);
}