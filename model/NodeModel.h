//
// Created by Micheal on 2026/5/1.
//

#ifndef NODEFORGE_NODEMODEL_H
#define NODEFORGE_NODEMODEL_H

#include <vector>
#include <QVariant>

class NodeModel {
public:
    virtual ~NodeModel() = default;
    virtual QString caption() const = 0; // 节点名称
    virtual void compute() = 0;         // 计算逻辑

    // 数据接口：输入端和输出端的数据存储
    std::vector<QVariant> inputs;
    std::vector<QVariant> outputs;
};

// 数字输入节点
class NumberNode : public NodeModel {
public:
    NumberNode(double val) { outputs.push_back(val); }
    QString caption() const override { return "Number"; }
    void compute() override { /* 输入节点不需要计算 */ }
    void setValue(double val) { outputs[0] = val; }
};

// 加法节点
class AddNode : public NodeModel {
public:
    AddNode() { inputs.assign(2, 0.0); outputs.push_back(0.0); }
    QString caption() const override { return "Add"; }
    void compute() override {
        double res = inputs[0].toDouble() + inputs[1].toDouble();
        outputs[0] = res;
    }
};

#endif //NODEFORGE_NODEMODEL_H