#include <QApplication>
#include <QFile>
#include <QDebug>
#include "ui/MainWindow.h"

static void loadStyleSheet(QApplication& app)
{
    const QString path = "D:/QtProject/NodeForge/ui/style/style.qss";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        app.setStyleSheet(QString::fromUtf8(file.readAll()));
    } else {
        qWarning() << "Failed to load style sheet:" << path;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    loadStyleSheet(a);

    MainWindow w;
    w.show();
    return a.exec();
}
