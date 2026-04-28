//
// Created by Micheal on 2026/4/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_NodeView.h" resolved

#include "NodeView.h"


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