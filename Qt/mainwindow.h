#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QThread>
#include <QPixmap>

extern "C" {
#include <Core/joypad.h>
}

class Screen;
class QPaintEvent;
class GameBoyWorker;
class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

signals:
	void start(QString filename);
	void keyEvent(GB_key_t index, bool pressed);

protected:
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void paintEvent(QPaintEvent * event);

private slots:
	void selectRom();
	void updateFrame(QPixmap pixmap);
	void stopEmulation();

private:
	void createMenu();
	void handleKey(int key, bool pressed);
	void loadRom(QString filename);
	
	QThread thread;
	QPixmap frame;
	GameBoyWorker * worker = nullptr;
};

#endif
