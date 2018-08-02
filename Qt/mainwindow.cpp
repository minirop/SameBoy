#include "mainwindow.h"
#include "gameboyworker.h"
#include "globals.h"

#include <QApplication>
#include <QAction>
#include <QKeyEvent>
#include <QDebug>
#include <QPainter>
#include <QFileDialog>

MainWindow::MainWindow()
{
	setFixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	setContextMenuPolicy(Qt::ActionsContextMenu);

	createMenu();

	worker = new GameBoyWorker;
	worker->moveToThread(&thread);
	connect(qApp, &QApplication::lastWindowClosed, worker, &GameBoyWorker::stop);
	connect(&thread, &QThread::started, worker, &GameBoyWorker::run);
	connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
	connect(worker, &GameBoyWorker::rendered, this, &MainWindow::updateFrame);
	connect(this, &MainWindow::keyEvent, worker, &GameBoyWorker::keyEvent);
	connect(this, &MainWindow::start, worker, &GameBoyWorker::loadRom);

	//selectRom();
}

MainWindow::~MainWindow()
{
	stopEmulation();
}

void MainWindow::createMenu()
{
	auto act = new QAction(QString("&Open rom"), this);
	connect(act, &QAction::triggered, this, &MainWindow::selectRom);
	addAction(act);
	act = new QAction(QString("&Quit"), this);
	connect(act, &QAction::triggered, this, &MainWindow::close);
	addAction(act);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	handleKey(event->key(), true);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
	handleKey(event->key(), false);
}

void MainWindow::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.drawPixmap(rect(), frame);
}

void MainWindow::updateFrame(QPixmap pixmap)
{
	frame = pixmap;
	update();
}

void MainWindow::handleKey(int key, bool pressed)
{
	switch (key)
	{
	case Qt::Key_Return:
		emit keyEvent(GB_KEY_START, pressed);
		break;
	case Qt::Key_Backspace:
		emit keyEvent(GB_KEY_SELECT, pressed);
		break;
	case Qt::Key_Up:
		emit keyEvent(GB_KEY_UP, pressed);
		break;
	case Qt::Key_Down:
		emit keyEvent(GB_KEY_DOWN, pressed);
		break;
	case Qt::Key_Left:
		emit keyEvent(GB_KEY_LEFT, pressed);
		break;
	case Qt::Key_Right:
		emit keyEvent(GB_KEY_RIGHT, pressed);
		break;
	case Qt::Key_Space:
		emit keyEvent(GB_KEY_A, pressed);
		break;
	case Qt::Key_D:
		emit keyEvent(GB_KEY_B, pressed);
		break;
	}
}

void MainWindow::selectRom()
{
	QString filename = QFileDialog::getOpenFileName(this, "Select rom", QApplication::applicationDirPath(), "Gameboy roms (*.gb *.gbc)");
	loadRom(filename);
}

void MainWindow::loadRom(QString filename)
{
	emit start(filename);
	thread.start();
}

void MainWindow::stopEmulation()
{
	worker->stop();
	thread.quit();
	thread.wait(1000);
}
