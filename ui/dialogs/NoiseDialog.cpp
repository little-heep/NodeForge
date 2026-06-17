//
// Created by Micheal on 2026/6/17.
//

#include "NoiseDialog.h"
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QVBoxLayout>

NoiseDialog::NoiseDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("噪声参数");
    resize(320, 180);

    auto* mainLayout = new QVBoxLayout(this);
    auto* form = new QFormLayout();

    m_meanSpin = new QDoubleSpinBox(this);
    m_meanSpin->setRange(-1e9, 1e9);
    m_meanSpin->setDecimals(6);

    m_sigmaSpin = new QDoubleSpinBox(this);
    m_sigmaSpin->setRange(0.0, 1e9);
    m_sigmaSpin->setDecimals(6);

    m_sampleCountSpin = new QSpinBox(this);
    m_sampleCountSpin->setRange(2, 100000);

    form->addRow("均值:", m_meanSpin);
    form->addRow("标准差:", m_sigmaSpin);
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

void NoiseDialog::setValues(double mean,
                            double sigma,
                            int sampleCount)
{
    m_meanSpin->setValue(mean);
    m_sigmaSpin->setValue(sigma);
    m_sampleCountSpin->setValue(sampleCount);
}

void NoiseDialog::getValues(double& mean,
                            double& sigma,
                            int& sampleCount) const
{
    mean = m_meanSpin->value();
    sigma = m_sigmaSpin->value();
    sampleCount = m_sampleCountSpin->value();
}