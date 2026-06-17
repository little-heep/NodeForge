#ifndef NODEFORGE_LOWPASSFILTERNODE_H
#define NODEFORGE_LOWPASSFILTERNODE_H

#include "../NodeModel.h"
#include <QInputDialog>
#include <QtMath>

// 一阶低通滤波器
class LowPassFilterNode : public NodeModel {
public:
    LowPassFilterNode(double alpha = 0.2)
        : m_alpha(alpha) {
        inputs.assign(1, QVariant());
        outputs.push_back(QVariant());
    }

    QString caption() const override { return "Low Pass"; }
    QString typeName() const override { return "LowPassFilterNode"; }

    void compute() override {
        QVariantList in = inputs.size() > 0 ? inputs[0].toList() : QVariantList();
        QVariantList out;
        if (in.isEmpty()) {
            outputs[0] = out;
            return;
        }

        out.reserve(in.size());

        double y = in[0].toDouble();
        out.append(y);

        for (int i = 1; i < in.size(); ++i) {
            const double x = in[i].toDouble();
            y = m_alpha * x + (1.0 - m_alpha) * y;
            out.append(y);
        }

        outputs[0] = out;
    }

    bool isEditable() const override { return true; }

    bool editValue(QWidget* parent) override {
        bool ok = false;
        double a = QInputDialog::getDouble(parent, "低通滤波参数", "Alpha (0~1)",
                                           m_alpha, 0.0, 1.0, 4, &ok);
        if (!ok) return false;
        m_alpha = a;
        return true;
    }

    QJsonObject toJson() const override {
        QJsonObject o = NodeModel::toJson();
        o["alpha"] = m_alpha;
        return o;
    }

    void fromJson(const QJsonObject &o) override {
        NodeModel::fromJson(o);
        if (o.contains("alpha")) m_alpha = o["alpha"].toDouble(m_alpha);
        if (inputs.empty()) inputs.assign(1, QVariant());
        if (outputs.empty()) outputs.push_back(QVariant());
    }

private:
    double m_alpha = 0.2;
};

#endif // NODEFORGE_LOWPASSFILTERNODE_H