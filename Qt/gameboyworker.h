#ifndef GAMEBOYWORKER_H
#define GAMEBOYWORKER_H

#include <QObject>
#include <QPixmap>

extern "C" {
#include <Core/gb.h>
}

class GameBoyWorker : public QObject
{
	Q_OBJECT

public:
	GameBoyWorker();
	void render(QPixmap pixmap);

public slots:
	void loadRom(QString filename);
	void run();
	void stop();
	void keyEvent(GB_key_t index, bool pressed);

signals:
	void rendered(QPixmap pixmap);

private:
	GB_gameboy_t gb;
	bool running = false;
};

#endif
