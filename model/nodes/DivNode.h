//
// Created by Micheal on 2026/5/19.
//

#ifndef NODEFORGE_DIVNODE_H
#define NODEFORGE_DIVNODE_H
#include "../NodeModel.h"
#include <QDebug>

// 除法节点
class DivNode : public NodeModel {
public:
    DivNode() { inputs.assign(2, 0.0); outputs.push_back(0.0); }

    QString caption() const override { return "Div"; }
    QString typeName() const override { return "DivNode"; }

    void compute() override {
        double denominator = inputs[1].toDouble();
        if (denominator == 0.0) {
            qWarning() << "DivNode: divide by zero";
            outputs[0] = 0.0;   // 也可以改成你想要的默认值
            return;
        }
        double res = inputs[0].toDouble() / denominator;
        outputs[0] = res;
    }
};
#endif //NODEFORGE_DIVNODE_H