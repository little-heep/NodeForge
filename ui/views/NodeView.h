//
// Created by Micheal on 2026/4/25.
//

#ifndef NODEFORGE_NODEVIEW_H
#define NODEFORGE_NODEVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>


class NodeView : public QGraphicsView {
    Q_OBJECT

public:
    explicit NodeView(QWidget *parent = nullptr);

    ~NodeView() override;
protected:
    // 滚轮缩放
    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

};


#endif //NODEFORGE_NODEVIEW_H