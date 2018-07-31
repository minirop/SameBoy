#include "mainwindow.h"
#include "gameboyworker.h"
#include <QPainter>
#include <QApplication>

MainWindow::MainWindow()
{
	auto worker = new GameBoyWorker;
	worker->moveToThread(&thread);
	connect(qApp, &QApplication::lastWindowClosed, worker, &GameBoyWorker::stop);
	connect(&thread, &QThread::started, worker, &GameBoyWorker::run);
	connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
	connect(worker, &GameBoyWorker::rendered, this, &MainWindow::updateCache);
	thread.start();
}

MainWindow::~MainWindow()
{
	thread.quit();
	thread.wait(1000);
}

void MainWindow::paintEvent(QPaintEvent * event)
{
    QPainter p(this);
    p.drawPixmap(rect(), cache);
}

void MainWindow::updateCache(QPixmap pixmap)
{
	cache = pixmap;
	update();
}
