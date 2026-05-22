//
// Created by Micheal on 2026/4/25.
//

#ifndef NODEFORGE_MAINWINDOW_H
#define NODEFORGE_MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPushButton>

#include "../core/NodeGraph.h"

class NodeScene;
class NodeView;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

private slots:
    void onFileNew();
    void onFileOpen();
    void onFileSave();
    void onRunClicked();
    void toggleComponentDock();

private:
    void setupMenuBar();
    void setupToolBar();
    void setupCentralArea();
    void setupComponentDock();

    NodeScene* m_scene = nullptr;
    NodeView* m_view = nullptr;
    QDockWidget* m_componentDock = nullptr;
    QAction* m_showDockAction = nullptr;
    QWidget* m_titleBar=nullptr;
    QWidget* central=nullptr;
    QPushButton* m_maxBtn = nullptr;
    NodeGraph m_graph;

    // --- 用于窗口拖动 ---
    bool m_dragging = false;
    QPoint m_dragPosition;
    bool eventFilter(QObject* obj, QEvent* event) override;

    // --- 用于窗口缩放 ---
    enum ResizeRegion {
        None, Left, Right, Top, Bottom,
        TopLeft, TopRight, BottomLeft, BottomRight
    };
    bool m_resizing = false;
    QPoint m_resizePressPos;
    QRect m_resizePressGeometry;
    ResizeRegion m_resizeRegion = None;
    void updateCursorShape(const QPoint& pos);
    void resizeWindow(const QPoint& globalPos);
    ResizeRegion hitTest(const QPoint& pos) const;


protected: // <<<<<<<<<<<<<<<<<<< 添加 protected 区域
    // 重写鼠标事件来实现窗口缩放
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


};


#endif //NODEFORGE_MAINWINDOW_H