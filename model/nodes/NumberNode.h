//
// Created by Micheal on 2026/5/19.
//

#ifndef NODEFORGE_NUMBERNODE_H
#define NODEFORGE_NUMBERNODE_H
#include "../NodeModel.h"
// 数字输入节点
class NumberNode : public NodeModel {
public:
    NumberNode(double val) { outputs.push_back(val); }
    QString caption() const override { return "Number"; }
    void compute() override { /* 输入节点不需要计算 */ }
    void setValue(double val) { outputs[0] = val; }
};

#endif //NODEFORGE_NUMBERNODE_H