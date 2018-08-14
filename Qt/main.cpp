#include <QApplication>
#include "mainwindow.h"

#include <QStyleFactory>
#include <QDebug>

Q_DECLARE_METATYPE(GB_key_t)

int main(int argc, char** argv)
{
    auto style = QStyleFactory::create("Windows");
    QApplication::setStyle(style);

    qRegisterMetaType<GB_key_t>();

    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
