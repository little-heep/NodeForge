//
// Created by Micheal on 2026/6/11.
//
#ifndef NODEFORGE_SINEWAVENODE_H
#define NODEFORGE_SINEWAVENODE_H

#include "../NodeModel.h"
#include "../ui/dialogs/SineWaveDialog.h"
#include <QtMath>

// 正弦波生成器节点：输出一个采样序列
class SineWaveNode : public NodeModel {
public:
    SineWaveNode(double frequency = 1.0,
                 double amplitude = 1.0,
                 double phase = 0.0,
                 double sampleRate = 100.0,
                 int sampleCount = 200)
        : m_frequency(frequency),
          m_amplitude(amplitude),
          m_phase(phase),
          m_sampleRate(sampleRate),
          m_sampleCount(sampleCount) {
        outputs.push_back(QVariant());
    }

    QString caption() const override { return "Sine"; }
    QString typeName() const override { return "SineWaveNode"; }
    QString showValue() override {
        return QString("f=%1Hz, A=%2, φ=%3rad, SR=%4Hz, N=%5")
                .arg(m_frequency).arg(m_amplitude).arg(m_phase).arg(m_sampleRate).arg(m_sampleCount);
    }

    void compute() override {
        QVariantList samples;
        samples.reserve(m_sampleCount);

        const double dt = 1.0 / m_sampleRate;
        for (int i = 0; i < m_sampleCount; ++i) {
            const double t = i * dt;
            const double y = m_amplitude * std::sin(2.0 * M_PI * m_frequency * t + m_phase);
            samples.append(y);
        }

        outputs[0] = samples;
    }

    bool isEditable() const override { return true; }

    bool editValue(QWidget* parent) override {
        SineWaveDialog dlg(parent);
        dlg.setValues(m_frequency, m_amplitude, m_phase, m_sampleRate, m_sampleCount);

        if (dlg.exec() != QDialog::Accepted) {
            return false;
        }

        dlg.getValues(m_frequency, m_amplitude, m_phase, m_sampleRate, m_sampleCount);
        return true;
    }

    QJsonObject toJson() const override {
        QJsonObject o = NodeModel::toJson();
        o["frequency"] = m_frequency;
        o["amplitude"] = m_amplitude;
        o["phase"] = m_phase;
        o["sampleRate"] = m_sampleRate;
        o["sampleCount"] = m_sampleCount;
        return o;
    }

    void fromJson(const QJsonObject &o) override {
        NodeModel::fromJson(o);
        if (o.contains("frequency")) m_frequency = o["frequency"].toDouble(m_frequency);
        if (o.contains("amplitude")) m_amplitude = o["amplitude"].toDouble(m_amplitude);
        if (o.contains("phase")) m_phase = o["phase"].toDouble(m_phase);
        if (o.contains("sampleRate")) m_sampleRate = o["sampleRate"].toDouble(m_sampleRate);
        if (o.contains("sampleCount")) m_sampleCount = o["sampleCount"].toInt(m_sampleCount);

        if (outputs.empty()) outputs.push_back(QVariant());
    }

private:
    double m_frequency = 1.0;
    double m_amplitude = 1.0;
    double m_phase = 0.0;
    double m_sampleRate = 100.0;
    int m_sampleCount = 200;
};

#endif // NODEFORGE_SINEWAVENODE_H