//
// Created by Micheal on 2026/4/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_NodeScene.h" resolved

#include "NodeScene.h"


NodeScene::NodeScene(QObject *parent) : QGraphicsScene(parent) {
    // 设置场景范围，可以设大一点
    setSceneRect(-5000, -5000, 10000, 10000);
}

NodeScene::~NodeScene() {

}
void NodeScene::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsScene::drawBackground(painter, rect);
    // 1. 背景底色
    painter->fillRect(rect, QColor(255, 254, 249));

    const int gridSize = 25;

    // 2. 找到当前可见区域的左、上、右、下边界
    qreal left = rect.left();
    qreal top = rect.top();

    // 3. 计算对齐后的起始位置 (核心逻辑)
    // fmod 是对浮点数取模，确保网格线在移动时不会跳动
    qreal firstLeft = std::floor(left / gridSize) * gridSize;
    qreal firstTop = std::floor(top / gridSize) * gridSize;

    // 4. 准备画笔
    QPen finePen(QColor(207, 206, 202), 1.0);
    QPen thickPen(QColor(173, 173, 169), 2.0); // 粗线用于大网格

    // 5. 绘制垂直线
    for (qreal x = firstLeft; x <= rect.right(); x += gridSize) {
        // 每隔 5 根线画一根粗线（可选）
        if (qAbs(static_cast<long>(x) % (gridSize * 5)) < 2) {
            painter->setPen(thickPen);
        } else {
            painter->setPen(finePen);
        }
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }

    // 6. 绘制水平线
    for (qreal y = firstTop; y <= rect.bottom(); y += gridSize) {
        if (qAbs(static_cast<long>(y) % (gridSize * 5)) < 2) {
            painter->setPen(thickPen);
        } else {
            painter->setPen(finePen);
        }
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }
}