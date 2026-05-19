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

    bool editValue(QWidget* parent) override {
        bool ok = false;
        double current = outputs.empty() ? 0.0 : outputs[0].toDouble();
        double v = QInputDialog::getDouble(parent, "修改 Number", "请输入数值：",
                                           current, -1e9, 1e9, 4, &ok);
        if (ok) setValue(v);
        return ok;
    }
};

#endif //NODEFORGE_NUMBERNODE_H