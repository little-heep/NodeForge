//
// Created by Micheal on 2026/5/19.
//

#ifndef NODEFORGE_ADDNODE_H
#define NODEFORGE_ADDNODE_H
#include "../NodeModel.h"
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

#endif //NODEFORGE_ADDNODE_H