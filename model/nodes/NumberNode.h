//
// Created by Micheal on 2026/5/19.
//

#ifndef NODEFORGE_NUMBERNODE_H
#define NODEFORGE_NUMBERNODE_H
#include "../NodeModel.h"
#include "QInputDialog"
// 数字输入节点
class NumberNode : public NodeModel {
public:
    NumberNode(double val) { outputs.push_back(val); }
    QString caption() const override { return "Number"; }
    void compute() override { /* 输入节点不需要计算 */ }
    void setValue(double val) { outputs[0] = val; }
    bool isEditable() const override { return true; }
    QString typeName() const override { return "NumberNode"; }

    bool editValue(QWidget* parent) override {
        bool ok = false;
        double current = outputs.empty() ? 0.0 : outputs[0].toDouble();
        double v = QInputDialog::getDouble(parent, "修改 Number", "请输入数值：",
                                           current, -1e9, 1e9, 4, &ok);
        if (ok) setValue(v);
        return ok;
    }

    QJsonObject toJson() const override {
        QJsonObject o = NodeModel::toJson();
        double v = outputs.empty() ? 0.0 : outputs[0].toDouble();
        o["value"] = v;
        return o;
    }
    void fromJson(const QJsonObject &o) override {
        NodeModel::fromJson(o);
        if (o.contains("value")) {
            double v = o["value"].toDouble();
            if (outputs.empty()) outputs.push_back(v);
            else outputs[0] = v;
        }
    }
};

#endif //NODEFORGE_NUMBERNODE_H