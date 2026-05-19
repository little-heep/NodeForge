//
// Created by Micheal on 2026/5/19.
//

#ifndef NODEFORGE_SUBNODE_H
#define NODEFORGE_SUBNODE_H
#include "../NodeModel.h"

// 减法节点
class SubNode : public NodeModel {
public:
    SubNode() { inputs.assign(2, 0.0); outputs.push_back(0.0); }

    QString caption() const override { return "Sub"; }

    void compute() override {
        double res = inputs[0].toDouble() - inputs[1].toDouble();
        outputs[0] = res;
    }
};

#endif //NODEFORGE_SUBNODE_H