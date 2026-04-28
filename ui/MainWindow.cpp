//
// Created by Micheal on 2026/4/25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "MainWindow.h"
#include "views/NodeScene.h"
#include "views/NodeView.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // 1. 创建场景
    NodeScene* scene = new NodeScene(this);

    // 2. 创建视图并把场景塞进去
    NodeView* view = new NodeView(this);
    view->setScene(scene);

    // 3. 设为中心组件
    setCentralWidget(view);

    resize(800, 600);
    setWindowTitle("Qt Node Editor");
}

MainWindow::~MainWindow() {

}