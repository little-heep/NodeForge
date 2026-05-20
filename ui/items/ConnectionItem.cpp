//
// Created by Micheal on 2026/4/28.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ConnectionItem.h" resolved

#include "ConnectionItem.h"

#include <QStyle>
#include <QStyleOptionGraphicsItem>

#include "PortItem.h"

ConnectionItem::ConnectionItem(QGraphicsItem *parent) : QGraphicsPathItem(parent) {
    setZValue(-1); // 确保线条在节点下面
    setAcceptHoverEvents(false);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setPen(QPen(QColor(180, 180, 180), 2)); // 默认颜色
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

void ConnectionItem::paint(QPainter *p, const QStyleOptionGraphicsItem *opt, QWidget *w) {
    QPen pen = this->pen();
    if (opt && (opt->state & QStyle::State_Selected)) {
        pen.setColor(QColor(255, 170, 0));
        pen.setWidthF(3.5);
    } else {
        pen.setColor(QColor(180, 180, 180));
        pen.setWidthF(2.0);
    }
    p->setPen(pen);
    p->setBrush(Qt::NoBrush);
    p->drawPath(path());
}