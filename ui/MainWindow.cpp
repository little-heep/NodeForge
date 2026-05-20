
#include "MainWindow.h"
#include "views/NodeScene.h"
#include "views/NodeView.h"
#include "items/NodeItem.h"
#include "../model/nodes/AddNode.h"
#include "../model/nodes/NumberNode.h"
#include "../model/nodes/SubNode.h"
#include "../model/nodes/StringNode.h"
#include "../model/nodes/MulNode.h"
#include "../model/nodes/DivNode.h"
#include "../model/nodes/StringAddNode.h"
#include "../model/nodes/CustomJsNode.h"
#include "items/PortItem.h"
#include "items/ConnectionItem.h"
#include "items/NodeItem.h"
#include "../model/NodeFactory.h"
#include "dialogs/CustomNodeDialog.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>

#include "../model/nodes/StringNode.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    resize(800, 600);
    setWindowTitle("Qt Node Editor");

    setupComponentDock(); // 创建组件面板（初始隐藏）
    setupMenuBar();      // 创建菜单栏
    setupCentralArea();  // 创建中心区域（scene和view）

    // 设置状态栏
    statusBar()->showMessage("就绪");
}

MainWindow::~MainWindow() {

}

void MainWindow::setupMenuBar()
{
    m_titleBar = new QWidget(this);
    m_titleBar->setFixedHeight(40);

    QHBoxLayout* titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(10, 0, 10, 0);
    titleLayout->setSpacing(10);

    // 左侧：Logo和标题
    QLabel* logoLabel = new QLabel("📊");
    titleLayout->addWidget(logoLabel);

    QLabel* titleLabel = new QLabel("节点编辑器");
    titleLayout->addWidget(titleLabel);

    // 添加分隔线
    QFrame* line1 = new QFrame();
    line1->setFrameShape(QFrame::VLine);
    titleLayout->addWidget(line1);

    // 文件操作按钮组
    QPushButton* newBtn = new QPushButton("新建");
    newBtn->setToolTip("新建文件 (Ctrl+N)");
    newBtn->setShortcut(QKeySequence::New);
    connect(newBtn, &QPushButton::clicked, this, &MainWindow::onFileNew);
    titleLayout->addWidget(newBtn);

    QPushButton* openBtn = new QPushButton("打开");
    openBtn->setToolTip("打开文件 (Ctrl+O)");
    openBtn->setShortcut(QKeySequence::Open);
    connect(openBtn, &QPushButton::clicked, this, &MainWindow::onFileOpen);
    titleLayout->addWidget(openBtn);

    QPushButton* saveBtn = new QPushButton("保存");
    saveBtn->setToolTip("保存文件 (Ctrl+S)");
    saveBtn->setShortcut(QKeySequence::Save);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onFileSave);
    titleLayout->addWidget(saveBtn);

    // 添加分隔线
    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::VLine);
    titleLayout->addWidget(line2);

    // 运行按钮（突出显示）
    QPushButton* runBtn = new QPushButton("▶ 运行");
    runBtn->setToolTip("执行节点图 (F5)");
    runBtn->setShortcut(QKeySequence(Qt::Key_F5));
    connect(runBtn, &QPushButton::clicked, this, &MainWindow::onRunClicked);
    titleLayout->addWidget(runBtn);

    // 添加分隔线
    QFrame* line3 = new QFrame();
    line3->setFrameShape(QFrame::VLine);
    line3->setStyleSheet("background-color: #7f8c8d;");
    titleLayout->addWidget(line3);

    // 组件库按钮
    QPushButton* componentBtn = new QPushButton("📦 组件库");
    componentBtn->setToolTip("显示/隐藏组件库 (Ctrl+D)");
    componentBtn->setShortcut(QKeySequence("Ctrl+D"));
    connect(componentBtn, &QPushButton::clicked, this, &MainWindow::toggleComponentDock);
    titleLayout->addWidget(componentBtn);

    // 弹性空间
    titleLayout->addStretch();

    // 右侧：窗口控制按钮
    QPushButton* minBtn = new QPushButton("—");
    minBtn->setFixedSize(30, 30);
    minBtn->setToolTip("最小化");
    connect(minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    titleLayout->addWidget(minBtn);

    QPushButton* maxBtn = new QPushButton("□");
    maxBtn->setFixedSize(30, 30);
    maxBtn->setToolTip("最大化/还原");
    connect(maxBtn, &QPushButton::clicked, [this]() {
        if (isMaximized())
            showNormal();
        else
            showMaximized();
    });
    titleLayout->addWidget(maxBtn);

    QPushButton* closeBtn = new QPushButton("×");
    closeBtn->setFixedSize(30, 30);
    closeBtn->setToolTip("关闭");
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
    titleLayout->addWidget(closeBtn);

}

void MainWindow::setupCentralArea()
{
    // 创建场景和视图
    m_scene = new NodeScene(this);
    m_view = new NodeView(this);
    m_view->setScene(m_scene);

    central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(m_titleBar);
    layout->addWidget(m_view);

    setCentralWidget(central);

}

void MainWindow::setupComponentDock()
{
    // 创建 Dock Widget
    m_componentDock = new QDockWidget("组件库", this);
    m_componentDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_componentDock->setFeatures(QDockWidget::DockWidgetClosable |
                                 QDockWidget::DockWidgetMovable |
                                 QDockWidget::DockWidgetFloatable);

    // 创建内容 Widget
    QWidget* dockContent = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(dockContent);

    // 组件列表
    QListWidget* componentList = new QListWidget();

    // 添加分类
    QListWidgetItem* categoryInput = new QListWidgetItem("输入节点");
    categoryInput->setFlags(Qt::NoItemFlags);  // 不可选择
    categoryInput->setBackground(QBrush(QColor(240, 240, 240)));
    componentList->addItem(categoryInput);

    QListWidgetItem* numberItem = new QListWidgetItem("   Number Node");
    numberItem->setData(Qt::UserRole, "NumberNode");
    componentList->addItem(numberItem);

    QListWidgetItem* stringItem = new QListWidgetItem("   String Node");
    stringItem->setData(Qt::UserRole, "StringNode");
    componentList->addItem(stringItem);

    QListWidgetItem* categoryString = new QListWidgetItem("字符运算");
    categoryString->setFlags(Qt::NoItemFlags);  // 不可选择
    categoryString->setBackground(QBrush(QColor(240, 240, 240)));
    componentList->addItem(categoryString);

    QListWidgetItem* spliceItem = new QListWidgetItem("   Splice Node");
    spliceItem->setData(Qt::UserRole, "SpliceNode");
    componentList->addItem(spliceItem);

    QListWidgetItem* categoryMath = new QListWidgetItem("数学运算");
    categoryMath->setFlags(Qt::NoItemFlags);
    categoryMath->setBackground(QBrush(QColor(240, 240, 240)));
    componentList->addItem(categoryMath);

    QListWidgetItem* addItem = new QListWidgetItem("   Add Node");
    addItem->setData(Qt::UserRole, "AddNode");
    componentList->addItem(addItem);

    QListWidgetItem* subItem = new QListWidgetItem("   Subtract Node");
    subItem->setData(Qt::UserRole, "SubtractNode");
    componentList->addItem(subItem);

    QListWidgetItem* mulItem = new QListWidgetItem("   Multiply Node");
    mulItem->setData(Qt::UserRole, "MultiplyNode");
    componentList->addItem(mulItem);

    QListWidgetItem* divItem = new QListWidgetItem("   Division Node");
    divItem->setData(Qt::UserRole, "DivisionNode");
    componentList->addItem(divItem);

    QListWidgetItem* customJsItem = new QListWidgetItem("   Custom JS Node");
    customJsItem->setData(Qt::UserRole, "CustomJsNode");
    componentList->addItem(customJsItem);

    layout->addWidget(componentList);

    // 说明文字
    QLabel* infoLabel = new QLabel("💡 双击组件添加到画布");
    infoLabel->setStyleSheet("color: gray; padding: 5px;");
    infoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoLabel);

    // 双击添加节点到场景
    connect(componentList, &QListWidget::itemDoubleClicked,
            [this, componentList](QListWidgetItem* item) {
        if (item->flags() == Qt::NoItemFlags) return;  // 分类项不可添加

        QString nodeType = item->data(Qt::UserRole).toString();
        if (nodeType == "NumberNode") {
            auto* numberNode = new NumberNode(2);
            m_graph.addNode(numberNode);
            auto* nodeItem = new NodeItem("Number",0,1, numberNode, &m_graph);
            nodeItem->setPos(0, 0);
            m_scene->addItem(nodeItem);
            statusBar()->showMessage("已添加 Number Node", 2000);
        } else if (nodeType == "AddNode") {
            auto* addNode = new AddNode();
            m_graph.addNode(addNode);
            auto* nodeItem = new NodeItem("Add",2,1, addNode, &m_graph);
            nodeItem->setPos(0, 0);
            m_scene->addItem(nodeItem);
            statusBar()->showMessage("已添加 Add Node", 2000);
        }else if (nodeType == "StringNode") {
            auto* stringNode = new StringNode("hello");
            m_graph.addNode(stringNode);
            auto* nodeItem = new NodeItem("String",0,1, stringNode, &m_graph);
            nodeItem->setPos(0, 0);
            m_scene->addItem(nodeItem);
            statusBar()->showMessage("已添加 String Node", 2000);
        }else if (nodeType == "MultiplyNode") {
            auto *mulNode = new MulNode();
            m_graph.addNode(mulNode);
            auto* nodeItem = new NodeItem("Multiply",2,1, mulNode, &m_graph);
            nodeItem->setPos(0, 0);
            m_scene->addItem(nodeItem);
            statusBar()->showMessage("已添加 Multiply Node", 2000);
        }else if (nodeType == "DivisionNode") {
            auto* divNode = new DivNode();
            m_graph.addNode(divNode);
            auto* nodeItem = new NodeItem("Division",2,1, divNode, &m_graph);
            nodeItem->setPos(0, 0);
            m_scene->addItem(nodeItem);
            statusBar()->showMessage("已添加 Division Node", 2000);
        }else if (nodeType == "SubtractNode") {
            auto* subNode = new SubNode();
            m_graph.addNode(subNode);
            auto* nodeItem = new NodeItem("Subtract",2,1, subNode, &m_graph);
            nodeItem->setPos(0, 0);
            m_scene->addItem(nodeItem);
            statusBar()->showMessage("已添加 Subtract Node", 2000);
        }else if (nodeType == "SpliceNode") {
            auto* spliceNode=new StringAddNode();
            m_graph.addNode(spliceNode);
            auto* nodeItem = new NodeItem("Concat",2,1, spliceNode, &m_graph);
            nodeItem->setPos(0, 0);
            m_scene->addItem(nodeItem);
            statusBar()->showMessage("已添加 Concat Node", 2000);
        }else if (nodeType == "CustomJsNode") {
            CustomNodeDialog dlg(this);
            if (dlg.exec() != QDialog::Accepted) {
                return;
            }

            CustomNodeConfig cfg = dlg.config();

            auto* customNode = new CustomJsNode(cfg.nodeName, cfg.inputCount, cfg.outputCount, cfg.jsCode);
            m_graph.addNode(customNode);

            auto* nodeItem = new NodeItem(cfg.nodeName, cfg.inputCount, cfg.outputCount, customNode, &m_graph);
            nodeItem->setPos(0, 0);
            m_scene->addItem(nodeItem);

            statusBar()->showMessage("已添加 Custom JS Node", 2000);
        }
    });

    m_componentDock->setWidget(dockContent);

    // 初始隐藏
    m_componentDock->hide();
}

void MainWindow::toggleComponentDock()
{
    if (m_componentDock->isVisible()) {
        m_componentDock->hide();
    } else {
        // 添加到左侧并显示
        addDockWidget(Qt::LeftDockWidgetArea, m_componentDock);
        m_componentDock->show();
    }
}

void MainWindow::onFileNew()
{
    // 清空场景
    m_scene->clear();
    m_graph.clear();
    statusBar()->showMessage("新建文件", 2000);
}

void MainWindow::onFileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开文件", "", "Node Files (*.json)");
    if (fileName.isEmpty()) return;

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "打开失败", "无法打开文件: " + fileName);
        return;
    }
    QByteArray data = f.readAll();
    f.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::warning(this, "打开失败", "文件格式不正确");
        return;
    }
    QJsonObject root = doc.object();

    // 清空现有场景和数据
    m_scene->clear();
    m_graph.clear();

    // 临时映射： id -> NodeModel*, id -> NodeItem*
    QHash<int, NodeModel*> idToModel;
    QHash<int, NodeItem*> idToItem;

    // 1) 读取 nodes
    if (root.contains("nodes") && root["nodes"].isArray()) {
        QJsonArray nodesArr = root["nodes"].toArray();
        for (const QJsonValue &v : nodesArr) {
            if (!v.isObject()) continue;
            QJsonObject no = v.toObject();

            // 使用 NodeFactory 创建 model（NodeFactory::createNodeFromJson 已实现）
            NodeModel* model = NodeFactory::createNodeFromJson(no);
            if (!model) {
                qWarning() << "Failed to create model for node json:" << no;
                continue;
            }

            // 如果 JSON 根对象有 id 字段，NodeFactory 已设置 model->id()
            m_graph.addNode(model);

            // 从 model 得到输入/输出端口数量（使用 inputs/outputs 长度）
            int inCount = static_cast<int>(model->inputs.size());
            int outCount = static_cast<int>(model->outputs.size());

            // NodeItem constructor: NodeItem(const QString &title, int inputsize,int outputsize,NodeModel* model, NodeGraph* graph, QGraphicsItem *parent = nullptr);
            NodeItem* ni = new NodeItem(model->caption(), inCount, outCount, model, &m_graph);
            // position 在 JSON 中可能以 node["pos"] 保存
            if (no.contains("pos") && no["pos"].isObject()) {
                QJsonObject pos = no["pos"].toObject();
                double x = pos.contains("x") ? pos["x"].toDouble() : 0.0;
                double y = pos.contains("y") ? pos["y"].toDouble() : 0.0;
                ni->setPos(x, y);
            } else {
                ni->setPos(0,0);
            }

            m_scene->addItem(ni);

            int id = no.contains("id") ? no["id"].toInt() : model->id();
            if (id == 0) id = model->id(); // fallback
            idToModel[id] = model;
            idToItem[id] = ni;
        }
    }

    // 2) 读取 connections 并重建（同时更新 data-layer 的连接）
    if (root.contains("connections") && root["connections"].isArray()) {
        QJsonArray connsArr = root["connections"].toArray();
        for (const QJsonValue &cv : connsArr) {
            if (!cv.isObject()) continue;
            QJsonObject co = cv.toObject();
            int outId = co["out"].toInt(-1);
            int outIdx = co["outIdx"].toInt(0);
            int inId = co["in"].toInt(-1);
            int inIdx = co["inIdx"].toInt(0);

            NodeModel* outM = idToModel.value(outId, nullptr);
            NodeModel* inM  = idToModel.value(inId, nullptr);
            NodeItem* outItem = idToItem.value(outId, nullptr);
            NodeItem* inItem  = idToItem.value(inId, nullptr);

            if (!outM || !inM || !outItem || !inItem) {
                qWarning() << "Skipping connection with missing nodes: outId" << outId << "inId" << inId;
                continue;
            }

            // 先在 graph 数据层加入连接
            m_graph.addConnection(outM, outIdx, inM, inIdx);

            // 在 UI 上重建 ConnectionItem：
            // helper：在 NodeItem 的 childItems 中找到对应类型和索引的 PortItem
            auto findPort = [](NodeItem* nodeItem, PortItem::PortType type, int idx)->PortItem* {
                for (QGraphicsItem* child : nodeItem->childItems()) {
                    if (auto p = dynamic_cast<PortItem*>(child)) {
                        if (p->portType() == type && p->index() == idx) return p;
                    }
                }
                return nullptr;
            };

            PortItem* sp = findPort(outItem, PortItem::Output, outIdx);
            PortItem* ep = findPort(inItem, PortItem::Input, inIdx);
            if (!sp || !ep) {
                qWarning() << "Port not found for connection: outId" << outId << "outIdx" << outIdx << "inId" << inId << "inIdx" << inIdx;
                continue;
            }

            ConnectionItem* conn = new ConnectionItem();
            conn->setStartPort(sp);
            conn->setEndPort(ep);
            // 注册到端口的列表
            sp->addConnection(conn);
            ep->addConnection(conn);
            m_scene->addItem(conn);
            conn->updatePath();
        }
    }

    // 3) 可选择：执行图计算并刷新显示
    m_graph.execute();
    for (QGraphicsItem* item : m_scene->items()) {
        if (auto nodeItem = dynamic_cast<NodeItem*>(item)) nodeItem->update();
    }

    statusBar()->showMessage("已加载: " + fileName, 2000);
}

void MainWindow::onFileSave()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存文件", "", "Node Files (*.json)");
    if (!fileName.isEmpty()) {
        // 保存文件逻辑
        m_view->saveToFile(fileName);
        statusBar()->showMessage("保存文件: " + fileName, 2000);
    }
}

void MainWindow::onRunClicked()
{
    statusBar()->showMessage("运行节点图...", 2000);
    m_graph.execute();

    // 刷新整个场景，让 NodeItem 重新 paint
    for (QGraphicsItem* item : m_scene->items()) {
        if (auto nodeItem = dynamic_cast<NodeItem*>(item)) {
            nodeItem->update();
        }
    }
}