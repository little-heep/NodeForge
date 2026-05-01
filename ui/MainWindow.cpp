
#include "MainWindow.h"
#include "views/NodeScene.h"
#include "views/NodeView.h"
#include "items/NodeItem.h"
#include "../model/NodeModel.h"
#include "../core/NodeGraph.h"
#include <QPushButton>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // 1. 创建场景
    NodeScene* scene = new NodeScene(this);

    // 2. 创建视图并把场景塞进去
    NodeView* view = new NodeView(this);
    view->setScene(scene);

    // 3. 设为中心组件
    setCentralWidget(view);
    /*
        NodeItem* node1 = new NodeItem("Number Node");
        node1->setPos(-150, 0);
        scene->addItem(node1);

        NodeItem* node2 = new NodeItem("Add Node");
        node2->setPos(50, 0);
        scene->addItem(node2);
    */
    NodeGraph graph;

    // 创建 Model
    auto n1 = new NumberNode(2);
    auto n2 = new NumberNode(3);
    auto n3 = new AddNode();
    graph.m_nodes = {n1, n2, n3};

    // 创建 UI 并关联
    auto item1 = new NodeItem("input1",n1);
    auto item2 = new NodeItem("input2",n2);
    auto item3 = new NodeItem("addresult",n3);
    graph.addConnection(n1, 0, n3, 0);
    graph.addConnection(n2, 0, n3, 1);

    scene->addItem(item1);
    scene->addItem(item2);
    scene->addItem(item3);

    // 点击按钮执行
    graph.execute();

    // 通知 UI 刷新显示内容
    item3->update();


    resize(800, 600);
    setWindowTitle("Qt Node Editor");
}

MainWindow::~MainWindow() {

}