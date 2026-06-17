
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
#include "../model/NodeFactory.h"
#include "dialogs/CustomNodeDialog.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include "GraphEditorContext.h"
#include "../model/nodes/SineWaveNode.h"
#include "../model/nodes/NoiseNode.h"
#include "../model/nodes/SignalAddNode.h"
#include "../model/nodes/LowPassFilterNode.h"
#include "../model/nodes/PlotNode.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    resize(800, 600);

     // 去除系统标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setMouseTracking(true);

    m_undoStack = new QUndoStack(this);
    m_undoStack->setUndoLimit(200); // 限制撤销历史长度
    setupComponentDock(); // 创建组件面板（初始隐藏）
    setupMenuBar();      // 创建菜单栏
    setupCentralArea();  // 创建中心区域（scene和view）

    m_editor = new GraphEditorContext(m_scene, &m_graph, m_undoStack, this);
    connect(m_view, &NodeView::deleteRequested, this, &MainWindow::onDeleteRequested);

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

    QLabel* titleLabel = new QLabel("NodeForge");
    titleLayout->addWidget(titleLabel);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
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

    //撤销、重做
    m_undoAction = m_undoStack->createUndoAction(this, tr("撤销"));
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_undoAction->setShortcutContext(Qt::ApplicationShortcut);
    addAction(m_undoAction);

    m_redoAction = m_undoStack->createRedoAction(this, tr("重做"));
    m_redoAction->setShortcut(QKeySequence::Redo);
    m_redoAction->setShortcutContext(Qt::ApplicationShortcut);
    addAction(m_redoAction);

    // 可选：按钮点击仍然保留
    auto* undoBtn = new QPushButton("↶ 撤销");
    undoBtn->setToolTip("撤销 (Ctrl+Z)");
    connect(undoBtn, &QPushButton::clicked, m_undoAction, &QAction::trigger);
    titleLayout->addWidget(undoBtn);

    auto* redoBtn = new QPushButton("↷ 重做");
    redoBtn->setToolTip("重做 (Ctrl+Y)");
    connect(redoBtn, &QPushButton::clicked, m_redoAction, &QAction::trigger);
    titleLayout->addWidget(redoBtn);

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

    m_maxBtn = new QPushButton("□");
    m_maxBtn->setFixedSize(30, 30);
    m_maxBtn->setToolTip("最大化/还原");
    connect(m_maxBtn, &QPushButton::clicked, [this]() {
        if (isMaximized())
            showNormal();
        else
            showMaximized();
    });
    titleLayout->addWidget(m_maxBtn);

    QPushButton* closeBtn = new QPushButton("×");
    closeBtn->setFixedSize(30, 30);
    closeBtn->setToolTip("关闭");
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
    titleLayout->addWidget(closeBtn);

    // 让 MainWindow 捕获 m_titleBar 的鼠标事件用于拖动窗口
    m_titleBar->setCursor(Qt::SizeAllCursor);
    m_titleBar->installEventFilter(this);
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
    QFont catFont = categoryInput->font();
    catFont.setBold(true);
    categoryInput->setFont(catFont);
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
    categoryString->setFont(catFont);
    componentList->addItem(categoryString);

    QListWidgetItem* spliceItem = new QListWidgetItem("   字符拼接");
    spliceItem->setData(Qt::UserRole, "SpliceNode");
    componentList->addItem(spliceItem);

    QListWidgetItem* categoryMath = new QListWidgetItem("数学运算");
    categoryMath->setFlags(Qt::NoItemFlags);
    categoryMath->setBackground(QBrush(QColor(240, 240, 240)));
    categoryMath->setFont(catFont);
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

    QListWidgetItem* categorySignal = new QListWidgetItem("信号分析");
    categorySignal->setFlags(Qt::NoItemFlags);
    categorySignal->setBackground(QBrush(QColor(240, 240, 240)));
    categorySignal->setFont(catFont);
    componentList->addItem(categorySignal);

    QListWidgetItem* sineItem = new QListWidgetItem("   正弦波生成器");
    sineItem->setData(Qt::UserRole, "SineWaveNode");
    componentList->addItem(sineItem);

    QListWidgetItem* noiseItem = new QListWidgetItem("   噪声发生器");
    noiseItem->setData(Qt::UserRole, "NoiseNode");
    componentList->addItem(noiseItem);

    QListWidgetItem* signalAddItem = new QListWidgetItem("   信号相加器");
    signalAddItem->setData(Qt::UserRole, "SignalAddNode");
    componentList->addItem(signalAddItem);

    QListWidgetItem* lowPassItem = new QListWidgetItem("   低通滤波器");
    lowPassItem->setData(Qt::UserRole, "LowPassFilterNode");
    componentList->addItem(lowPassItem);

    QListWidgetItem* plotItem = new QListWidgetItem("   折线图显示");
    plotItem->setData(Qt::UserRole, "PlotNode");
    componentList->addItem(plotItem);

    QListWidgetItem* categoryCustom = new QListWidgetItem("自定义运算");
    categoryCustom->setFlags(Qt::NoItemFlags);
    categoryCustom->setBackground(QBrush(QColor(240, 240, 240)));
    categoryCustom->setFont(catFont);
    componentList->addItem(categoryCustom);

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
            [this](QListWidgetItem* item) {
                if (item->flags() == Qt::NoItemFlags) return;  // 分类项不可添加

                QString nodeType = item->data(Qt::UserRole).toString();
                QPointF centerPos = m_view->mapToScene(m_view->viewport()->rect().center());
                if (nodeType == "NumberNode") {
                    // auto* numberNode = new NumberNode(2);
                    // m_graph.addNode(numberNode);
                    // auto* nodeItem = new NodeItem("Number",0,1, numberNode, &m_graph);
                    // nodeItem->setPos(centerPos);
                    // m_scene->addItem(nodeItem);
                    QJsonObject nodeObj;
                    nodeObj["type"] = "NumberNode";
                    QJsonObject data;
                    data["value"] = 0;
                    nodeObj["data"] = data;

                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Number Node", 2000);
                } else if (nodeType == "AddNode") {
                    QJsonObject nodeObj;
                    nodeObj["type"] = "AddNode";
                    QJsonObject data;
                    data["value"] = 0;
                    nodeObj["data"] = data;

                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Add Node", 2000);
                }else if (nodeType == "StringNode") {
                    QJsonObject nodeObj;
                    nodeObj["type"] = "StringNode";
                    QJsonObject data;
                    data["value"] = "hello";
                    nodeObj["data"] = data;

                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 String Node", 2000);
                }else if (nodeType == "MultiplyNode") {
                    QJsonObject nodeObj;
                    nodeObj["type"] = "MulNode";
                    QJsonObject data;
                    nodeObj["data"] = data;

                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Multiply Node", 2000);
                }else if (nodeType == "DivisionNode") {
                    QJsonObject nodeObj;
                    nodeObj["type"] = "DivNode";
                    QJsonObject data;
                    nodeObj["data"] = data;

                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Division Node", 2000);
                }else if (nodeType == "SubtractNode") {
                    QJsonObject nodeObj;
                    nodeObj["type"] = "SubNode";
                    QJsonObject data;
                    nodeObj["data"] = data;

                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Subtract Node", 2000);
                }else if (nodeType == "SpliceNode") {
                    QJsonObject nodeObj;
                    nodeObj["type"] = "StringAddNode";
                    QJsonObject data;
                    nodeObj["data"] = data;

                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Concat Node", 2000);
                }else if (nodeType == "CustomJsNode") {
                    CustomNodeDialog dlg(this);
                    if (dlg.exec() != QDialog::Accepted) {
                        return;
                    }

                    CustomNodeConfig cfg = dlg.config();

                    // auto* customNode = new CustomJsNode(cfg.nodeName, cfg.inputCount, cfg.outputCount, cfg.jsCode);
                    // m_graph.addNode(customNode);
                    //
                    // auto* nodeItem = new NodeItem(cfg.nodeName, cfg.inputCount, cfg.outputCount, customNode, &m_graph);
                    //QPointF centerPos = m_view->mapToScene(m_view->viewport()->rect().center());
                    // nodeItem->setPos(centerPos);
                    // m_scene->addItem(nodeItem);
                    QJsonObject nodeObj;
                    nodeObj["type"] = "CustomJsNode";
                    QJsonObject data;
                    data["name"] = cfg.nodeName;
                    data["inputCount"] = cfg.inputCount;
                    data["outputCount"] = cfg.outputCount;
                    data["jsCode"] = cfg.jsCode;
                    nodeObj["data"] = data;

                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Custom JS Node", 2000);
                } else if (nodeType == "SineWaveNode") {
                    QJsonObject nodeObj;
                    nodeObj["type"] = "SineWaveNode";
                    QJsonObject data;
                    data["frequency"] = 1.0;
                    data["amplitude"] = 1.0;
                    data["phase"] = 0.0;
                    data["sampleRate"] = 100.0;
                    data["sampleCount"] = 200;
                    nodeObj["data"] = data;
                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Sine Wave Node", 2000);

                } else if (nodeType == "NoiseNode") {
                    QJsonObject nodeObj;
                    nodeObj["type"] = "NoiseNode";
                    QJsonObject data;
                    data["mean"] = 0.0;
                    data["sigma"] = 0.2;
                    data["sampleCount"] = 200;
                    nodeObj["data"] = data;
                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Noise Node", 2000);

                } else if (nodeType == "SignalAddNode") {
                    QJsonObject nodeObj;
                    nodeObj["type"] = "SignalAddNode";
                    QJsonObject data;
                    nodeObj["data"] = data;
                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Signal Add Node", 2000);

                } else if (nodeType == "LowPassFilterNode") {
                    QJsonObject nodeObj;
                    nodeObj["type"] = "LowPassFilterNode";
                    QJsonObject data;
                    data["alpha"] = 0.2;
                    nodeObj["data"] = data;
                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Low Pass Node", 2000);

                } else if (nodeType == "PlotNode") {
                    QJsonObject nodeObj;
                    nodeObj["type"] = "PlotNode";
                    QJsonObject data;
                    nodeObj["data"] = data;
                    m_editor->requestAddNode(nodeObj, centerPos);
                    statusBar()->showMessage("已添加 Plot Node", 2000);
                };
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
    m_editor->clearDocument();
    m_undoStack->clear();
    m_undoStack->setClean();
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

    m_editor->rebuildFromJson(root);
    m_undoStack->clear();
    m_undoStack->setClean();

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

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !isMaximized()) {
        m_resizeRegion = hitTest(event->pos());
        if (m_resizeRegion != None) {
            m_resizing = true;
            m_resizePressPos = event->globalPos();
            m_resizePressGeometry = geometry();
            event->accept();
            return;
        }
    }
    // 如果不是缩放操作，则调用基类实现
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_resizing) {
        resizeWindow(event->globalPos());
        event->accept();
    } else {
        // 只有在不拖动标题栏、不缩放窗口时才更新光标
        if (!m_dragging) {
            updateCursorShape(event->pos());
        }
        QMainWindow::mouseMoveEvent(event);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_resizing) {
        m_resizing = false;
        m_resizeRegion = None;
        // 释放后恢复默认光标
        updateCursorShape(event->pos());
    }
    QMainWindow::mouseReleaseEvent(event);
}

// 判断鼠标位置在哪个缩放区域
MainWindow::ResizeRegion MainWindow::hitTest(const QPoint& pos) const
{
    if (isMaximized()) {
        return None; // 最大化时不允许缩放
    }

    const int margin = 8; // 边缘检测的像素宽度
    const QRect rect = this->rect();

    bool onLeft = pos.x() >= rect.left() && pos.x() < rect.left() + margin;
    bool onRight = pos.x() > rect.right() - margin && pos.x() <= rect.right();
    bool onTop = pos.y() >= rect.top() && pos.y() < rect.top() + margin;
    bool onBottom = pos.y() > rect.bottom() - margin && pos.y() <= rect.bottom();

    if (onTop && onLeft) return TopLeft;
    if (onTop && onRight) return TopRight;
    if (onBottom && onLeft) return BottomLeft;
    if (onBottom && onRight) return BottomRight;
    if (onLeft) return Left;
    if (onRight) return Right;
    if (onTop) return Top;
    if (onBottom) return Bottom;

    return None;
}

// 根据鼠标位置更新光标形状
void MainWindow::updateCursorShape(const QPoint& pos)
{
    if (isMaximized()) {
        unsetCursor();
        return;
    }

    switch (hitTest(pos)) {
        case Left:
        case Right:
            setCursor(Qt::SizeHorCursor);
            break;
        case Top:
        case Bottom:
            setCursor(Qt::SizeVerCursor);
            break;
        case TopLeft:
        case BottomRight:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case TopRight:
        case BottomLeft:
            setCursor(Qt::SizeBDiagCursor);
            break;
        default:
            unsetCursor(); // 不在边缘时恢复默认光标
            break;
    }
}

// 根据鼠标拖动调整窗口大小
void MainWindow::resizeWindow(const QPoint& globalPos)
{
    QRect newGeometry = m_resizePressGeometry;
    QPoint delta = globalPos - m_resizePressPos;

    const int minW = minimumWidth();
    const int minH = minimumHeight();

    switch (m_resizeRegion) {
        case Left:
            newGeometry.setLeft(m_resizePressGeometry.left() + delta.x());
            if (newGeometry.width() < minW) newGeometry.setLeft(newGeometry.right() - minW);
            break;
        case Right:
            newGeometry.setRight(m_resizePressGeometry.right() + delta.x());
            if (newGeometry.width() < minW) newGeometry.setRight(newGeometry.left() + minW);
            break;
        case Top:
            newGeometry.setTop(m_resizePressGeometry.top() + delta.y());
            if (newGeometry.height() < minH) newGeometry.setTop(newGeometry.bottom() - minH);
            break;
        case Bottom:
            newGeometry.setBottom(m_resizePressGeometry.bottom() + delta.y());
            if (newGeometry.height() < minH) newGeometry.setBottom(newGeometry.top() + minH);
            break;
        case TopLeft:
            newGeometry.setTopLeft(m_resizePressGeometry.topLeft() + delta);
            if (newGeometry.width() < minW) newGeometry.setLeft(newGeometry.right() - minW);
            if (newGeometry.height() < minH) newGeometry.setTop(newGeometry.bottom() - minH);
            break;
        case TopRight:
            newGeometry.setTopRight(m_resizePressGeometry.topRight() + delta);
            if (newGeometry.width() < minW) newGeometry.setRight(newGeometry.left() + minW);
            if (newGeometry.height() < minH) newGeometry.setTop(newGeometry.bottom() - minH);
            break;
        case BottomLeft:
            newGeometry.setBottomLeft(m_resizePressGeometry.bottomLeft() + delta);
            if (newGeometry.width() < minW) newGeometry.setLeft(newGeometry.right() - minW);
            if (newGeometry.height() < minH) newGeometry.setBottom(newGeometry.top() + minH);
            break;
        case BottomRight:
            newGeometry.setBottomRight(m_resizePressGeometry.bottomRight() + delta);
            if (newGeometry.width() < minW) newGeometry.setRight(newGeometry.left() + minW);
            if (newGeometry.height() < minH) newGeometry.setBottom(newGeometry.top() + minH);
            break;
        default:
            break;
    }
    setGeometry(newGeometry);
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // 只处理标题栏的事件
    if (obj == m_titleBar) {
        auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (!mouseEvent) {
            return QMainWindow::eventFilter(obj, event);
        }
        switch (event->type()) {
            case QEvent::MouseButtonDblClick:
                // 双击最大化/还原
                m_maxBtn->click(); // 模拟点击最大化按钮
                return true;

            case QEvent::MouseButtonPress:
                if (mouseEvent->button() == Qt::LeftButton) {
                    // 如果点击在按钮上，则不处理，让按钮响应
                    if (m_titleBar->childAt(mouseEvent->pos())) {
                        return false;
                    }
                    // 如果窗口已最大化，则不允许拖动
                    if (isMaximized()) {
                        return true;
                    }
                    m_dragging = true;
                    m_dragPosition = mouseEvent->globalPos() - this->pos();
                    return true;
                }
                break;

            case QEvent::MouseMove:
                if (m_dragging && (mouseEvent->buttons() & Qt::LeftButton)) {
                    move(mouseEvent->globalPos() - m_dragPosition);
                    return true;
                }
                break;

            case QEvent::MouseButtonRelease:
                if (mouseEvent->button() == Qt::LeftButton) {
                    m_dragging = false;
                    return true;
                }
                break;

            default:
                break;
        }
    }
    // 其他对象的事件交给基类处理
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::onDeleteRequested()
{
    if (!m_view || !m_view->scene()) return;
    const auto selected = m_view->scene()->selectedItems();
    if (selected.isEmpty()) return;

    m_editor->requestDeleteSelection(selected);
}
