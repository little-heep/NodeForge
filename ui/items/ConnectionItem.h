//
// Created by Micheal on 2026/4/28.
//

#ifndef NODEFORGE_CONNECTIONITEM_H
#define NODEFORGE_CONNECTIONITEM_H

#include <QGraphicsPathItem>

class PortItem; // 前向声明

class ConnectionItem : public QGraphicsPathItem {
public:
    ConnectionItem(QGraphicsItem *parent = nullptr);

    void setStartPort(PortItem* port) { m_startPort = port; }
    void setEndPort(PortItem* port) { m_endPort = port; m_isTemp = false; }
    void setEndPoint(const QPointF& pos) { m_endPoint = pos; m_isTemp = true; }

    // 关键：刷新曲线形状
    void updatePath();

    PortItem* startPort() const { return m_startPort; }
    PortItem* endPort() const { return m_endPort; }

private:
    PortItem* m_startPort = nullptr;
    PortItem* m_endPort = nullptr;
    QPointF m_endPoint; // 仅用于拖拽时的临时终点
    bool m_isTemp = false;

protected:
    void paint(QPainter *p, const QStyleOptionGraphicsItem *opt, QWidget *w) override;

};

#endif //NODEFORGE_CONNECTIONITEM_H