//
// Created by Micheal on 2026/4/28.
//

#ifndef NODEFORGE_PORTITEM_H
#define NODEFORGE_PORTITEM_H

#include <QGraphicsItem>
#include <QPainter>

class ConnectionItem;
class NodeModel;
class NodeGraph;

class PortItem : public QGraphicsItem {

public:
    enum PortType { Input, Output };
    PortItem(PortType type, NodeModel* model, NodeGraph* graph,int index,QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override { return QRectF(-6, -6, 12, 12); } // 以中心点绘图
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override {
        painter->setPen(QPen(Qt::white, 1));
        painter->setBrush(QColor(100, 200, 100)); // 绿色端口
        painter->drawEllipse(boundingRect());
    }

    void updateConnections();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    PortType m_type;
    QPointF m_pos;
    QPointF m_end;
    bool m_isTemp;
    NodeModel* m_model = nullptr;//所属model
    int m_index;//端口索引
    ConnectionItem *m_tempConn=nullptr;
    QList<ConnectionItem*> m_connections;
    NodeGraph* m_graph = nullptr;
};


#endif //NODEFORGE_PORTITEM_H