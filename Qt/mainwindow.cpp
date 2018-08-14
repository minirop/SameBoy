#include "mainwindow.h"
#include "gameboyworker.h"
#include "globals.h"

#include <QApplication>
#include <QAction>
#include <QKeyEvent>
#include <QDebug>
#include <QPainter>
#include <QFileDialog>
#include <QSignalMapper>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

using VoidActionFunc = void(QAction::*)();
using VoidMapperFunc = void(QSignalMapper::*)();

MainWindow::MainWindow()
    : vramViewer(this)
{
    setWindowTitle("QameBoy");
    resizeWindow(1);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);

    connect(this, &MainWindow::customContextMenuRequested, this, &MainWindow::showContextMenu);

    createMenu();

    worker = new GameBoyWorker;
    worker->moveToThread(&thread);
    connect(qApp, &QApplication::lastWindowClosed, worker, &GameBoyWorker::stop);
    connect(&thread, &QThread::started, worker, &GameBoyWorker::run);
    connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &GameBoyWorker::rendered, this, &MainWindow::updateFrame);
    connect(worker, &GameBoyWorker::gbState, &vramViewer, &VramViewer::updateMemory);
    connect(this, &MainWindow::keyEvent, worker, &GameBoyWorker::keyEvent);
    connect(this, &MainWindow::setTurbo, worker, &GameBoyWorker::setTurbo);
    connect(this, &MainWindow::start, worker, &GameBoyWorker::loadRom);

    vramViewer.hide();
}

MainWindow::~MainWindow()
{
    stopEmulation();
}

void MainWindow::createMenu()
{
    QAction * act = nullptr;

    contextMenu.addAction("&Pause")->setEnabled(false);
    contextMenu.addAction("&Load ROM...", this, SLOT(selectRom()));
    contextMenu.addAction("&Enable sound")->setEnabled(false);
    contextMenu.addAction("&Options...")->setEnabled(false);
    contextMenu.addAction("&Cheat...")->setEnabled(false);
    act = contextMenu.addAction("&Reset gameboy");
    connect(act, &QAction::triggered, [this]() {
        worker->reset();
    });

    contextMenu.addMenu("S&tate")->setEnabled(false);

    // OTHER
    auto otherMenu = contextMenu.addMenu("Ot&her");
    act = otherMenu->addAction("&VRAM Viewer");
    connect(act, &QAction::triggered, [this]() {
        this->vramViewer.show();
    });


    contextMenu.addMenu("So&und channel")->setEnabled(false);

    auto windowSizeMapper = new QSignalMapper(this);
    connect(windowSizeMapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &MainWindow::resizeWindow);
    auto windowSizeActionGroup = new QActionGroup(this);

    // WINDOW SIZE
    auto windowSizeMenu = contextMenu.addMenu("&Window size");

    act = windowSizeMenu->addAction("&1x1");
    act->setCheckable(true);
    act->setChecked(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 1);

    act = windowSizeMenu->addAction("&2x2");
    act->setCheckable(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 2);

    act = windowSizeMenu->addAction("&3x3");
    act->setCheckable(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 3);

    act = windowSizeMenu->addAction("&4x4");
    act->setCheckable(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 4);

    act = windowSizeMenu->addAction("&5x5");
    act->setCheckable(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 5);

    act = windowSizeMenu->addAction("&6x6");
    act->setCheckable(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 6);

    act = windowSizeMenu->addAction("&Full screen");
    act->setCheckable(true);
    act->setEnabled(false);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 7);

    act = windowSizeMenu->addAction("F&ullscreen stretch");
    act->setCheckable(true);
    act->setEnabled(false);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 8);

    contextMenu.addMenu("L&ink")->setEnabled(false);

    // RECENT ROMS
    recentRomsMapper = new QSignalMapper(this);
    connect(recentRomsMapper, static_cast<void(QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped), this, &MainWindow::loadRom);

    recentRomsMenu = contextMenu.addMenu("Rece&nt ROMs");
    rebuildRecentRomsMenu();

    contextMenu.addAction("E&xit", this, SLOT(close()));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat())
        handleKey(event->key(), true);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat())
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
    case Qt::Key_M:
        emit setTurbo(pressed);
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
    if (thread.isRunning())
    {
        QMessageBox::warning(this, "Can't load another ROM", "For now, once a ROM has been loaded, you need to exit Qameboy to play another game.");
        return;
    }

    if (filename.isEmpty()) return;

    settings.addRecentRom(filename);

    rebuildRecentRomsMenu();

    emit start(filename);
    thread.start();
}

void MainWindow::stopEmulation()
{
    worker->stop();
    thread.quit();
    thread.wait(1000);

    frame = QPixmap();
}

void MainWindow::showContextMenu(const QPoint & pos)
{
    contextMenu.exec(mapToGlobal(pos));
}

void MainWindow::resizeWindow(int i)
{
    if (i < 7)
    {
        setFixedSize(SCREEN_WIDTH*i, SCREEN_HEIGHT*i);
    }
}

void MainWindow::rebuildRecentRomsMenu()
{
    recentRomsMenu->clear();

    auto recentRoms = settings.getRecentRoms();
    if (recentRoms.size() > 0)
    {
        for (auto & filename : recentRoms)
        {
            auto act = recentRomsMenu->addAction(filename);
            connect(act, &QAction::triggered, recentRomsMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
            recentRomsMapper->setMapping(act, filename);
        }
    }
    else
    {
        auto act = recentRomsMenu->addAction("Empty...");
        act->setEnabled(false);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent * event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent * event)
{
    QUrl file = event->mimeData()->urls().first();
    if (file.isLocalFile())
    {
        if (file.fileName().endsWith(".gb") || file.fileName().endsWith(".gbc"))
        {
            QString filename = file.toString();
            if (filename.startsWith("file://"))
            {
                filename = filename.mid(7);
                filename = QUrl::fromPercentEncoding(filename.toLocal8Bit());
                loadRom(filename);
            }
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    vramViewer.close();
}
