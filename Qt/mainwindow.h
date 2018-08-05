#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QThread>
#include <QPixmap>
#include <QMenu>

#include "settings.h"
#include "vramviewer.h"

class QPaintEvent;
class QDragEnterEvent;
class QDropEvent;
class QSignalMapper;
class Screen;
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
	void keyPressEvent(QKeyEvent * event);
	void keyReleaseEvent(QKeyEvent * event);
	void paintEvent(QPaintEvent * event);
	void dragEnterEvent(QDragEnterEvent * event);
	void dropEvent(QDropEvent * event);
	void closeEvent(QCloseEvent * event);

private slots:
	void selectRom();
	void updateFrame(QPixmap pixmap);
	void stopEmulation();
	void showContextMenu(const QPoint & pos);
	void resizeWindow(int i);

private:
	void createMenu();
	void handleKey(int key, bool pressed);
	void loadRom(QString filename);
	void rebuildRecentRomsMenu();

	QThread thread;
	QPixmap frame;
	GameBoyWorker * worker = nullptr;
	QMenu contextMenu;
	QMenu * recentRomsMenu = nullptr;
	QSignalMapper * recentRomsMapper = nullptr;
	Settings settings;
	VramViewer vramViewer;
};

#endif
