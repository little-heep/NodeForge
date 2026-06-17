#ifndef NODEFORGE_SIGNALADDNODE_H
#define NODEFORGE_SIGNALADDNODE_H

#include "../NodeModel.h"
#include <algorithm>

// 信号相加器：两路序列逐点相加
class SignalAddNode : public NodeModel {
public:
    SignalAddNode() {
        inputs.assign(2, QVariant());
        outputs.push_back(QVariant());
    }

    QString caption() const override { return "Signal Add"; }
    QString typeName() const override { return "SignalAddNode"; }

    void compute() override {
        QVariantList a = inputs.size() > 0 ? inputs[0].toList() : QVariantList();
        QVariantList b = inputs.size() > 1 ? inputs[1].toList() : QVariantList();

        const int n = std::min(a.size(), b.size());
        QVariantList out;
        out.reserve(n);

        for (int i = 0; i < n; ++i) {
            const double av = a[i].toDouble();
            const double bv = b[i].toDouble();
            out.append(av + bv);
        }

        outputs[0] = out;
    }

    void fromJson(const QJsonObject &o) override {
        Q_UNUSED(o);
        inputs.assign(2, QVariant());
        outputs.assign(1, QVariant());
    }
};

#endif // NODEFORGE_SIGNALADDNODE_H