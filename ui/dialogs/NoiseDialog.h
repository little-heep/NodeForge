//
// Created by Micheal on 2026/6/17.
//

#ifndef NODEFORGE_NOISEDIALOG_H
#define NODEFORGE_NOISEDIALOG_H


#include <QDialog>

class QDoubleSpinBox;
class QSpinBox;

class NoiseDialog : public QDialog {
    Q_OBJECT
public:
    explicit NoiseDialog(QWidget* parent = nullptr);

    void setValues(double mean,
                   double sigma,
                   int sampleCount);

    void getValues(double& mean,
                   double& sigma,
                   int& sampleCount) const;

private:
    QDoubleSpinBox* m_meanSpin = nullptr;
    QDoubleSpinBox* m_sigmaSpin = nullptr;
    QSpinBox* m_sampleCountSpin = nullptr;
};


#endif //NODEFORGE_NOISEDIALOG_H