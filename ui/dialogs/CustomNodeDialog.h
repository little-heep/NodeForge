//
// Created by Micheal on 2026/5/19.
//

#ifndef NODEFORGE_CUSTOMNODEDIALOG_H
#define NODEFORGE_CUSTOMNODEDIALOG_H


#include <QDialog>
#include <QString>

class QLineEdit;
class QSpinBox;
class QTextEdit;

struct CustomNodeConfig {
    QString nodeName;
    int inputCount = 1;
    int outputCount = 1;
    QString jsCode;
};

class CustomNodeDialog : public QDialog {
    Q_OBJECT
public:
    explicit CustomNodeDialog(QWidget* parent = nullptr);

    CustomNodeConfig config() const;

private slots:
    void onAccept();

private:
    QLineEdit* m_nameEdit = nullptr;
    QSpinBox* m_inputSpin = nullptr;
    QSpinBox* m_outputSpin = nullptr;
    QTextEdit* m_codeEdit = nullptr;
};

#endif //NODEFORGE_CUSTOMNODEDIALOG_H