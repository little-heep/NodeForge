#include "SineWaveDialog.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>

SineWaveDialog::SineWaveDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("正弦波参数");
    resize(360, 220);

    auto* mainLayout = new QVBoxLayout(this);

    auto* form = new QFormLayout();

    m_frequencySpin = new QDoubleSpinBox(this);
    m_frequencySpin->setRange(0.0, 1e9);
    m_frequencySpin->setDecimals(6);
    m_frequencySpin->setSuffix(" Hz");

    m_amplitudeSpin = new QDoubleSpinBox(this);
    m_amplitudeSpin->setRange(-1e9, 1e9);
    m_amplitudeSpin->setDecimals(6);

    m_phaseSpin = new QDoubleSpinBox(this);
    m_phaseSpin->setRange(-1e9, 1e9);
    m_phaseSpin->setDecimals(6);
    m_phaseSpin->setSuffix(" rad");

    m_sampleRateSpin = new QDoubleSpinBox(this);
    m_sampleRateSpin->setRange(1.0, 1e9);
    m_sampleRateSpin->setDecimals(6);
    m_sampleRateSpin->setSuffix(" Hz");

    m_sampleCountSpin = new QSpinBox(this);
    m_sampleCountSpin->setRange(2, 100000);

    form->addRow("频率 (Hz):", m_frequencySpin);
    form->addRow("幅值:", m_amplitudeSpin);
    form->addRow("初相位 (rad):", m_phaseSpin);
    form->addRow("采样率 (Hz):", m_sampleRateSpin);
    form->addRow("采样点数:", m_sampleCountSpin);

    mainLayout->addLayout(form);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        this
    );
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttons);
}

void SineWaveDialog::setValues(double frequency,
                               double amplitude,
                               double phase,
                               double sampleRate,
                               int sampleCount)
{
    m_frequencySpin->setValue(frequency);
    m_amplitudeSpin->setValue(amplitude);
    m_phaseSpin->setValue(phase);
    m_sampleRateSpin->setValue(sampleRate);
    m_sampleCountSpin->setValue(sampleCount);
}

void SineWaveDialog::getValues(double& frequency,
                               double& amplitude,
                               double& phase,
                               double& sampleRate,
                               int& sampleCount) const
{
    frequency = m_frequencySpin->value();
    amplitude = m_amplitudeSpin->value();
    phase = m_phaseSpin->value();
    sampleRate = m_sampleRateSpin->value();
    sampleCount = m_sampleCountSpin->value();
}