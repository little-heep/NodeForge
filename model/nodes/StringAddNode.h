//
// Created by Micheal on 2026/5/19.
//

#ifndef NODEFORGE_STRINGADDNODE_H
#define NODEFORGE_STRINGADDNODE_H
#include "../NodeModel.h"
#include <QString>

// 字符串连接节点
class StringAddNode : public NodeModel {
public:
    StringAddNode() {
        inputs.assign(2, QVariant());
        outputs.push_back(QString());
    }

    QString caption() const override { return "Concat"; }
    QString typeName() const override { return "StringAddNode"; }

    void compute() override {
        QString left = inputs[0].toString();
        QString right = inputs[1].toString();
        outputs[0] = left + right;
    }
};
#endif //NODEFORGE_STRINGADDNODE_H