#ifndef NODEFORGE_NOISENODE_H
#define NODEFORGE_NOISENODE_H

#include "../NodeModel.h"
#include "../ui/dialogs/NoiseDialog.h"
#include <QRandomGenerator>
#include <QtMath>

// 噪声发生器节点：输出随机噪声序列
class NoiseNode : public NodeModel {
public:
    NoiseNode(double mean = 0.0,
              double sigma = 0.2,
              int sampleCount = 200)
        : m_mean(mean),
          m_sigma(sigma),
          m_sampleCount(sampleCount) {
        outputs.push_back(QVariant());
    }

    QString caption() const override { return "Noise"; }
    QString typeName() const override { return "NoiseNode"; }
    QString showValue() override {
        return QString("均值=%1, 标准差=%2, 采样点数=%3")
                .arg(m_mean).arg(m_sigma).arg(m_sampleCount);
    }

    void compute() override {
        QVariantList samples;
        samples.reserve(m_sampleCount);

        // Box-Muller 生成高斯噪声
        for (int i = 0; i < m_sampleCount; ++i) {
            double u1 = qMax(1e-12, QRandomGenerator::global()->generateDouble());
            double u2 = QRandomGenerator::global()->generateDouble();
            double z0 = qSqrt(-2.0 * qLn(u1)) * qCos(2.0 * M_PI * u2);
            double v = m_mean + m_sigma * z0;
            samples.append(v);
        }

        outputs[0] = samples;
    }

    bool isEditable() const override { return true; }

    bool editValue(QWidget* parent) override {
        NoiseDialog dlg(parent);
        dlg.setValues(m_mean, m_sigma, m_sampleCount);

        if (dlg.exec() != QDialog::Accepted) {
            return false;
        }

        dlg.getValues(m_mean, m_sigma, m_sampleCount);
        return true;
    }

    QJsonObject toJson() const override {
        QJsonObject o = NodeModel::toJson();
        o["mean"] = m_mean;
        o["sigma"] = m_sigma;
        o["sampleCount"] = m_sampleCount;
        return o;
    }

    void fromJson(const QJsonObject &o) override {
        NodeModel::fromJson(o);
        if (o.contains("mean")) m_mean = o["mean"].toDouble(m_mean);
        if (o.contains("sigma")) m_sigma = o["sigma"].toDouble(m_sigma);
        if (o.contains("sampleCount")) m_sampleCount = o["sampleCount"].toInt(m_sampleCount);

        if (outputs.empty()) outputs.push_back(QVariant());
    }

private:
    double m_mean = 0.0;
    double m_sigma = 0.2;
    int m_sampleCount = 200;
};

#endif // NODEFORGE_NOISENODE_H