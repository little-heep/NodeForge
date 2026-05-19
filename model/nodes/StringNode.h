//
// Created by Micheal on 2026/5/19.
//

#ifndef NODEFORGE_STRINGNODE_H
#define NODEFORGE_STRINGNODE_H
#include "../NodeModel.h"
#include <QString>
#include <QInputDialog>
#include <QLineEdit>

// 字符串输入节点
class StringNode : public NodeModel {
public:
    StringNode(QString val) { outputs.push_back(val); }
    QString caption() const override { return "string"; }
    void compute() override {}
    void setValue(QString val) { outputs[0] = val; }

    bool isEditable() const override { return true; }

    bool editValue(QWidget* parent) override {
        bool ok = false;
        QString current = outputs.empty() ? QString() : outputs[0].toString();
        QString text = QInputDialog::getText(parent, "修改 String", "请输入文本：",
                                             QLineEdit::Normal, current, &ok);
        if (ok) setValue(text);
        return ok;
    }
};

#endif //NODEFORGE_STRINGNODE_H