#ifndef VRAMVIEWER_H
#define VRAMVIEWER_H

#include <QTabWidget>

extern "C" {
#include <Core/gb.h>
}

class QLabel;
class VramViewer : public QTabWidget
{
	Q_OBJECT

public:
	VramViewer();

public slots:
	void updateMemory(GB_gameboy_t * gb);

private:
	QLabel * backgroundLabel;
};

#endif
