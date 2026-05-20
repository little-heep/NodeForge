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
    QString typeName() const override { return "StringNode"; }

    bool isEditable() const override { return true; }

    bool editValue(QWidget* parent) override {
        bool ok = false;
        QString current = outputs.empty() ? QString() : outputs[0].toString();
        QString text = QInputDialog::getText(parent, "修改 String", "请输入文本：",
                                             QLineEdit::Normal, current, &ok);
        if (ok) setValue(text);
        return ok;
    }

    QJsonObject toJson() const override {
        QJsonObject o = NodeModel::toJson();
        QString v = outputs.empty() ? QString() : outputs[0].toString();
        o["value"] = v;
        return o;
    }
    void fromJson(const QJsonObject &o) override {
        NodeModel::fromJson(o);
        if (o.contains("value")) {
            QString v = o["value"].toString();
            if (outputs.empty()) outputs.push_back(v);
            else outputs[0] = v;
        }
    }
};

#endif //NODEFORGE_STRINGNODE_H