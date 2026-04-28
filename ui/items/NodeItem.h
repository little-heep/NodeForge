//
// Created by Micheal on 2026/4/28.
//

#ifndef NODEFORGE_NODEITEM_H
#define NODEFORGE_NODEITEM_H

#include <QGraphicsObject>
#include <QPainter>

class NodeItem : public QGraphicsObject {
    Q_OBJECT
public:
    explicit NodeItem(const QString &title, QGraphicsItem *parent = nullptr);

    // 必须实现：定义碰撞和绘制区域
    QRectF boundingRect() const override;
    // 必须实现：绘制具体长相
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QString m_title;
    int m_width = 120;
    int m_height = 80;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};


#endif //NODEFORGE_NODEITEM_H