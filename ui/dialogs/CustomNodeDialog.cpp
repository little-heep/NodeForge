#include "CustomNodeDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QJSEngine>
#include <QJSValue>

CustomNodeDialog::CustomNodeDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("创建自定义 JS 节点");
    resize(560, 420);

    auto* root = new QVBoxLayout(this);
    auto* form = new QFormLayout();

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("例如: MyCustomNode");
    m_nameEdit->setText("CustomJS");

    m_inputSpin = new QSpinBox(this);
    m_inputSpin->setRange(0, 32);
    m_inputSpin->setValue(2);

    m_outputSpin = new QSpinBox(this);
    m_outputSpin->setRange(0, 32);
    m_outputSpin->setValue(1);

    form->addRow("节点名称:", m_nameEdit);
    form->addRow("输入端口数:", m_inputSpin);
    form->addRow("输出端口数:", m_outputSpin);

    root->addLayout(form);

    auto* hint = new QLabel("JS 需定义: function compute(inputs) { return [ ... ]; }", this);
    root->addWidget(hint);

    m_codeEdit = new QTextEdit(this);
    m_codeEdit->setPlainText(
        "function compute(inputs) {\n"
        "    // 示例：两输入求和\n"
        "    return [Number(inputs[0] || 0) + Number(inputs[1] || 0)];\n"
        "}\n"
    );
    root->addWidget(m_codeEdit, 1);

    auto* box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(box, &QDialogButtonBox::accepted, this, &CustomNodeDialog::onAccept);
    connect(box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(box);
}

CustomNodeConfig CustomNodeDialog::config() const {
    CustomNodeConfig cfg;
    cfg.nodeName = m_nameEdit->text().trimmed();
    cfg.inputCount = m_inputSpin->value();
    cfg.outputCount = m_outputSpin->value();
    cfg.jsCode = m_codeEdit->toPlainText();
    return cfg;
}

void CustomNodeDialog::onAccept() {
    CustomNodeConfig cfg = config();

    if (cfg.nodeName.isEmpty()) {
        QMessageBox::warning(this, "参数错误", "节点名称不能为空。");
        return;
    }

    if (cfg.jsCode.trimmed().isEmpty()) {
        QMessageBox::warning(this, "参数错误", "JS 代码不能为空。");
        return;
    }

    // 预检查：至少要有 compute 函数
    QJSEngine engine;
    QJSValue evalResult = engine.evaluate(cfg.jsCode);
    if (evalResult.isError()) {
        QMessageBox::warning(this, "JS 错误", "脚本解析失败:\n" + evalResult.toString());
        return;
    }

    QJSValue fn = engine.globalObject().property("compute");
    if (!fn.isCallable()) {
        QMessageBox::warning(this, "JS 错误", "必须定义 function compute(inputs) { ... }");
        return;
    }

    accept();
}
