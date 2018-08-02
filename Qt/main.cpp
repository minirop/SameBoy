#include <QApplication>
#include "mainwindow.h"

Q_DECLARE_METATYPE(GB_key_t)

int main(int argc, char** argv)
{
	qRegisterMetaType<GB_key_t>();

	QApplication app(argc, argv);
	MainWindow w;
	w.show();
	return app.exec();
}
