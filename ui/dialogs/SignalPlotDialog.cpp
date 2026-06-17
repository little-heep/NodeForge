#include "SignalPlotDialog.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "../../thirdparty/qcustomplot/qcustomplot.h"

SignalPlotDialog::SignalPlotDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Signal Plot");
    resize(700, 420);

    auto* layout = new QVBoxLayout(this);

    m_plot = new QCustomPlot(this);
    layout->addWidget(m_plot, 1);

    auto* closeBtn = new QPushButton("关闭", this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeBtn);
}

void SignalPlotDialog::setSamples(const QVariantList& samples, const QString& title) {
    setWindowTitle(title);

    QVector<double> x;
    QVector<double> y;
    x.reserve(samples.size());
    y.reserve(samples.size());

    for (int i = 0; i < samples.size(); ++i) {
        x.push_back(i);
        y.push_back(samples[i].toDouble());
    }

    m_plot->clearGraphs();
    m_plot->addGraph();
    m_plot->graph(0)->setData(x, y);
    m_plot->graph(0)->setPen(QPen(QColor("#2563eb"), 2));

    m_plot->xAxis->setLabel("Sample");
    m_plot->yAxis->setLabel("Amplitude");
    m_plot->xAxis->setRange(0, qMax(1, x.size() - 1));

    double ymin = 0.0, ymax = 1.0;
    if (!y.isEmpty()) {
        ymin = *std::min_element(y.begin(), y.end());
        ymax = *std::max_element(y.begin(), y.end());
        if (qFuzzyCompare(ymin, ymax)) {
            ymin -= 1.0;
            ymax += 1.0;
        }
    }
    m_plot->yAxis->setRange(ymin, ymax);

    m_plot->replot();
}