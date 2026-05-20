//
// Created by Micheal on 2026/4/28.
//

// You may need to build the project (run Qt uic code generator) to get "ui_NodeItem.h" resolved

#include "NodeItem.h"

#include <QApplication>
#include <QGraphicsScene>

#include "PortItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

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
void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {
    painter->setRenderHint(QPainter::Antialiasing, true);

    const bool selected = option && (option->state & QStyle::State_Selected);

    QRectF rect = boundingRect();
    QRectF shadowRect = rect.translated(2.5, 2.5);

    // 1) 阴影（轻微）
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 18)); // very subtle shadow
    painter->drawRoundedRect(shadowRect, 8, 8);

    // 2) 卡片主体（浅色系渐变）
    QLinearGradient grad(0, 0, 0, rect.height());
    grad.setColorAt(0.0, QColor("#ffffff"));
    grad.setColorAt(1.0, QColor("#f2f6fb"));
    painter->setBrush(grad);

    QColor borderColor = selected ? QColor("#60a5fa") : QColor("#d8e2ea");
    painter->setPen(QPen(borderColor, selected ? 2.2 : 1.0));
    painter->drawRoundedRect(rect, 8, 8);

    // 3) 顶部标题条（更浅的背景）
    QRectF titleRect = rect.adjusted(0, 0, 0, -rect.height() + 28);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor("#eef6ff"));
    painter->drawRoundedRect(titleRect.adjusted(0, 0, 0, 2), 8, 8);

    // 4) 标题文本（加粗）
    QFont titleFont = painter->font();
    titleFont.setBold(true);
    painter->setFont(titleFont);
    painter->setPen(QColor("#0f172a"));
    painter->drawText(titleRect.adjusted(10, 0, -10, 0),
                      Qt::AlignVCenter | Qt::AlignCenter,
                      m_title);

    // 5) 内容/输出文本（深色）
    painter->setFont(QFont(painter->font().family(), 10));
    painter->setPen(QColor("#10243a"));
    if (m_model && !m_model->outputs.empty()) {
        QString out = m_model->outputs[0].toString();
        painter->drawText(rect.adjusted(10, 34, -10, -8),
                          Qt::AlignCenter | Qt::TextWordWrap,
                          out);
    }

    // 6) 选中框强调（虚线）
    if (selected) {
        painter->setBrush(Qt::NoBrush);
        QPen p(QColor("#60a5fa"), 1.0, Qt::DashLine);
        p.setDashPattern({4,4});
        painter->setPen(p);
        painter->drawRoundedRect(rect.adjusted(3, 3, -3, -3), 6, 6);
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

void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    if (m_model && m_model->isEditable()) {
        QWidget* parentWidget = QApplication::activeWindow();
        if (!parentWidget && scene() && !scene()->views().isEmpty()) {
            parentWidget = scene()->views().first()->window();
        }

        if (m_model->editValue(parentWidget)) {
            update();
        }
        event->accept();
        return;
    }
    QGraphicsObject::mouseDoubleClickEvent(event);
}