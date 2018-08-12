#ifndef VRAMVIEWER_H
#define VRAMVIEWER_H

#include <QTabWidget>

extern "C" {
#include <Core/gb.h>
}

class QLabel;
class QLineEdit;
class VramViewer : public QTabWidget
{
	Q_OBJECT

public:
	VramViewer();

public slots:
	void updateMemory(GB_gameboy_t * gb);

protected:
    bool eventFilter(QObject * obj, QEvent * event);

private:
	QLabel * backgroundLabel;
    QLabel * backgroundTileLabel;
    QRect tileRect;
    QLineEdit * xLineEdit;
    QLineEdit * tileIdLineEdit;
    QLineEdit * yLineEdit;
    QLineEdit * attributeLineEdit;
};

#endif
