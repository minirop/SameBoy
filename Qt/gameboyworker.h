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
	~GameBoyWorker();

public slots:
	void loadRom(QString filename);
	void run();
	void stop();
	void keyEvent(GB_key_t index, bool pressed);
	void reset();

signals:
	void rendered(QPixmap pixmap);
	void gbState(GB_gameboy_t * gb);

private:
	void render(QPixmap pixmap);
	void grabState();

	GB_gameboy_t gb;
	bool running = false;
};

#endif
