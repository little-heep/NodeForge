//
// Created by Micheal on 2026/4/25.
//

#ifndef NODEFORGE_MAINWINDOW_H
#define NODEFORGE_MAINWINDOW_H

#include <QMainWindow>

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

};


#endif //NODEFORGE_MAINWINDOW_H