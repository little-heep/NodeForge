//
// Created by Micheal on 2026/5/19.
//

#ifndef NODEFORGE_STRINGNODE_H
#define NODEFORGE_STRINGNODE_H
#include <qstring.h>
#include "../NodeModel.h"

// 字符串输入节点
class StringNode : public NodeModel {
public:
    StringNode(QString val) { outputs.push_back(val); }
    QString caption() const override { return "string"; }
    void compute() override {}
    void setValue(QString val) { outputs[0] = val; }
};

#endif //NODEFORGE_STRINGNODE_H