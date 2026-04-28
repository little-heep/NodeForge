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
    // 中键平移需要在 mousePress/Move/Release 中组合，或者简单开启 DragMode

};


#endif //NODEFORGE_NODEVIEW_H