//
// Created by Micheal on 2026/4/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "MainWindow.h"
#include "views/NodeScene.h"
#include "views/NodeView.h"
#include "items/NodeItem.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // 1. 创建场景
    NodeScene* scene = new NodeScene(this);

    // 2. 创建视图并把场景塞进去
    NodeView* view = new NodeView(this);
    view->setScene(scene);

    // 3. 设为中心组件
    setCentralWidget(view);

    NodeItem* node1 = new NodeItem("Number Node");
    node1->setPos(-150, 0);
    scene->addItem(node1);

    NodeItem* node2 = new NodeItem("Add Node");
    node2->setPos(50, 0);
    scene->addItem(node2);

    resize(800, 600);
    setWindowTitle("Qt Node Editor");
}

MainWindow::~MainWindow() {

}