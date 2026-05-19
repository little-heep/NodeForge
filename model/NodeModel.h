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



#endif //NODEFORGE_NODEMODEL_H