#ifndef NODEFORGE_SIGNALPLOTDIALOG_H
#define NODEFORGE_SIGNALPLOTDIALOG_H

#include <QDialog>
#include <QVariantList>

class QCustomPlot;

class SignalPlotDialog : public QDialog {
    Q_OBJECT
public:
    explicit SignalPlotDialog(QWidget* parent = nullptr);

    void setSamples(const QVariantList& samples, const QString& title = "Signal Plot");

private:
    QCustomPlot* m_plot = nullptr;
};

#endif // NODEFORGE_SIGNALPLOTDIALOG_H