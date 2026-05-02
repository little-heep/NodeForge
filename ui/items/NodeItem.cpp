//
// Created by Micheal on 2026/4/28.
//

// You may need to build the project (run Qt uic code generator) to get "ui_NodeItem.h" resolved

#include "NodeItem.h"
#include "PortItem.h"

NodeItem::NodeItem(const QString &title,int inputsize,int outputsize, NodeModel* model, NodeGraph* graph, QGraphicsItem *parent)
    : m_model(model),m_input_size(inputsize),m_output_size(outputsize), m_graph(graph), QGraphicsObject(parent), m_title(title)
{
    setFlag(QGraphicsItem::ItemIsMovable);       // 开启拖拽
    setFlag(QGraphicsItem::ItemIsSelectable);    // 开启选中
    setFlag(QGraphicsItem::ItemSendsGeometryChanges); // 位置改变时发信号

    // 创建输入端口 (靠左)
    if (m_input_size > 0) {
        qreal spacing = static_cast<qreal>(m_height) / (m_input_size + 1);
        for (int i = 0; i < m_input_size; ++i) {
            auto* inPort = new PortItem(PortItem::Input, m_model, m_graph, i, this);
            qreal y = spacing * (i + 1); // 放在 1..n 的刻度上
            inPort->setPos(0, y);
        }
    }

    // 创建输出端口 (靠右)
    if (m_output_size > 0) {
        qreal spacingOut = static_cast<qreal>(m_height) / (m_output_size + 1);
        for (int i = 0; i < m_output_size; ++i) {
            auto* outPort = new PortItem(PortItem::Output, m_model, m_graph, i, this);
            qreal y = spacingOut * (i + 1);
            outPort->setPos(m_width, y);
        }
    }
}

QRectF NodeItem::boundingRect() const {
    return QRectF(0, 0, m_width, m_height);
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // 画阴影/背景
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 50));
    painter->drawRoundedRect(boundingRect().translated(3, 3), 10, 10);

    // 画主体圆角矩形
    QLinearGradient grad(0, 0, 0, m_height);
    grad.setColorAt(0, QColor(187, 232, 255));
    grad.setColorAt(1, QColor(167, 212, 235));
    painter->setBrush(grad);
    painter->setPen(QPen(QColor(156, 211, 245), 1.5));
    painter->drawRoundedRect(boundingRect(), 10, 10);

    // 画标题
    painter->setPen(QPen(Qt::white, 1));
    painter->drawText(boundingRect().adjusted(10, 5, -10, -5), Qt::AlignTop | Qt::AlignHCenter, m_title);

    // 额外画出 Model 里的数据
    if (!m_model->outputs.empty()) {
        painter->drawText(boundingRect().adjusted(10, 5, -10, -5), Qt::AlignCenter, m_model->outputs[0].toString());
    }
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged) {
        // 找到所有连接到这个节点的线，强制它们 updatePath
        for (QGraphicsItem *child : childItems()) {
            if (auto port = dynamic_cast<PortItem *>(child)) {
                port->updateConnections();
            }
        }
    }
    return QGraphicsObject::itemChange(change, value);
}