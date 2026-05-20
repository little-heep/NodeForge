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
    //setPen(QPen(QColor(180, 180, 180), 2)); // 默认颜色
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

void ConnectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {
    painter->setRenderHint(QPainter::Antialiasing, true);

    QColor normalColor("#9aa8b6");   // light gray-blue for default
    QColor hoverColor("#6ea8ff");    // light blue on hover
    QColor selectColor("#2f80ed");   // stronger blue for selected

    QPen pen;
    if (option && (option->state & QStyle::State_Selected)) {
        pen = QPen(selectColor, 3.2);
    } else if (option && (option->state & QStyle::State_MouseOver)) {
        pen = QPen(hoverColor, 2.6);
    } else {
        pen = QPen(normalColor, 2.0);
    }

    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    // optionally draw a subtle glow for selected
    if (option && (option->state & QStyle::State_Selected)) {
        QPen glow(QColor(47,128,237,40), 6);
        glow.setCapStyle(Qt::RoundCap);
        painter->setPen(glow);
        painter->drawPath(path());
        painter->setPen(pen);
    }

    painter->drawPath(path());
}