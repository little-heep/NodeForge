//
// Created by Micheal on 2026/4/25.
//

#ifndef NODEFORGE_NODESCENE_H
#define NODEFORGE_NODESCENE_H

#include <QGraphicsScene>
#include <QPainter>

class NodeScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit NodeScene(QObject *parent = nullptr);

    ~NodeScene() override;

protected:
    // 重写背景绘制
    void drawBackground(QPainter *painter, const QRectF &rect) override;

};


#endif //NODEFORGE_NODESCENE_H