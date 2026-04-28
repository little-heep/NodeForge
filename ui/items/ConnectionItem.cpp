//
// Created by Micheal on 2026/4/28.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ConnectionItem.h" resolved

#include "ConnectionItem.h"
#include "PortItem.h"

ConnectionItem::ConnectionItem(QGraphicsItem *parent) : QGraphicsPathItem(parent) {
    setZValue(-1); // 确保线条在节点下面
    setAcceptHoverEvents(false);
}

void ConnectionItem::updatePath() {
    if (!m_startPort) {
        return;
    }

    QPointF startPos = m_startPort->scenePos();
    QPointF endPos;

    if (m_isTemp) {
        endPos = m_endPoint;
    } else {
        if (!m_endPort) {
            return;
        }
        endPos = m_endPort->scenePos();
    }

    QPainterPath path;
    path.moveTo(startPos);

    qreal dx = endPos.x() - startPos.x();
    qreal ctrlX = qMax(40.0, qAbs(dx) * 0.5);

    path.cubicTo(startPos.x() + ctrlX, startPos.y(),
                 endPos.x() - ctrlX, endPos.y(),
                 endPos.x(), endPos.y());

    setPath(path);
}