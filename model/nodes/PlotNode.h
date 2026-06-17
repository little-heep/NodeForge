#ifndef NODEFORGE_PLOTNODE_H
#define NODEFORGE_PLOTNODE_H

#include "../NodeModel.h"
#include "../../ui/dialogs/SignalPlotDialog.h"

// 显示节点：只保存数据，不负责绘图UI
// 双击后由 UI 弹窗显示折线图
class PlotNode : public NodeModel {
public:
    PlotNode() {
        inputs.assign(1, QVariant());
    }

    QString caption() const override { return "Plot"; }
    QString typeName() const override { return "PlotNode"; }
    QString showValue() override {
        return "双击查看折线图";
    }

    void compute() override {
        //不进行计算
    }
    bool isEditable() const override { return true; }
    bool editValue(QWidget* parent) override {
        SignalPlotDialog dlg(parent);
        dlg.setSamples(inputs[0].toList(), "Plot Node Output");
        dlg.exec();
        return true;
    }

    void fromJson(const QJsonObject &o) override {
        NodeModel::fromJson(o);
        if (inputs.empty()) inputs.assign(1, QVariant());
    }
};

#endif // NODEFORGE_PLOTNODE_H