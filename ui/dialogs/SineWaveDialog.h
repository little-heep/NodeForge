//
// Created by Micheal on 2026/6/17.
//

#ifndef NODEFORGE_SINEWAVEDIALOG_H
#define NODEFORGE_SINEWAVEDIALOG_H

#include <QDialog>

class QDoubleSpinBox;
class QSpinBox;

class SineWaveDialog : public QDialog {
    Q_OBJECT
public:
    explicit SineWaveDialog(QWidget* parent = nullptr);

    void setValues(double frequency,
                   double amplitude,
                   double phase,
                   double sampleRate,
                   int sampleCount);

    void getValues(double& frequency,
                   double& amplitude,
                   double& phase,
                   double& sampleRate,
                   int& sampleCount) const;

private:
    QDoubleSpinBox* m_frequencySpin = nullptr;
    QDoubleSpinBox* m_amplitudeSpin = nullptr;
    QDoubleSpinBox* m_phaseSpin = nullptr;
    QDoubleSpinBox* m_sampleRateSpin = nullptr;
    QSpinBox* m_sampleCountSpin = nullptr;
};

#endif //NODEFORGE_SINEWAVEDIALOG_H