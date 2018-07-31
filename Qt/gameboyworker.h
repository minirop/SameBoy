#ifndef GAMEBOYWORKER_H
#define GAMEBOYWORKER_H

#include <QObject>
#include <QPixmap>

class GameBoyWorker : public QObject
{
	Q_OBJECT

public:
	GameBoyWorker();
	void render(QPixmap pixmap);

public slots:
	void run();
	void stop();

signals:
	void rendered(QPixmap pixmap);

private:
	bool running = false;
};

#endif
