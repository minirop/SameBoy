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

    act = contextMenu.addAction("&Pause");
    addAction(act);
    act->setEnabled(false);

    act = contextMenu.addAction("&Load ROM...", this, SLOT(selectRom()));
    addAction(act);

    act = contextMenu.addAction("&Enable sound");
    addAction(act);
    act->setEnabled(false);

    act = contextMenu.addAction("&Options...");
    act->setEnabled(false);
    addAction(act);

    act = contextMenu.addAction("&Cheat...");
    act->setEnabled(false);
    addAction(act);

    act = contextMenu.addAction("&Reset gameboy");
    addAction(act);
    act->setShortcut(QKeySequence("CTRL+R"));
    connect(act, &QAction::triggered, [this]() {
        worker->reset();
    });

    contextMenu.addMenu("S&tate")->setEnabled(false);

    // OTHER
    auto otherMenu = contextMenu.addMenu("Ot&her");
    act = otherMenu->addAction("&VRAM Viewer");
    addAction(act);
    act->setShortcut(QKeySequence("CTRL+V"));
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
    addAction(act);
    act->setCheckable(true);
    act->setChecked(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 1);

    act = windowSizeMenu->addAction("&2x2");
    addAction(act);
    act->setCheckable(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 2);

    act = windowSizeMenu->addAction("&3x3");
    addAction(act);
    act->setCheckable(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 3);

    act = windowSizeMenu->addAction("&4x4");
    addAction(act);
    act->setCheckable(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 4);

    act = windowSizeMenu->addAction("&5x5");
    addAction(act);
    act->setCheckable(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 5);

    act = windowSizeMenu->addAction("&6x6");
    addAction(act);
    act->setCheckable(true);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 6);

    act = windowSizeMenu->addAction("&Full screen");
    addAction(act);
    act->setCheckable(true);
    act->setEnabled(false);
    windowSizeActionGroup->addAction(act);
    connect(act, &QAction::triggered, windowSizeMapper, static_cast<VoidMapperFunc>(&QSignalMapper::map));
    windowSizeMapper->setMapping(act, 7);

    act = windowSizeMenu->addAction("F&ullscreen stretch");
    addAction(act);
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

    act = contextMenu.addAction("E&xit", this, SLOT(close()), QKeySequence("CTRL+Q"));
    addAction(act);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat())
        handleKey(event->key(), true);
    event->ignore();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat())
        handleKey(event->key(), false);
    event->ignore();
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
    auto gbKey = settings.getGbKey(key);
    if (gbKey != GB_KEY_MAX)
    {
        emit keyEvent(GB_KEY_START, pressed);
    }
    else
    {
        auto action = settings.getAction(key);
        switch (action)
        {
        case Settings::Action::Turbo:
            emit setTurbo(pressed);
            break;
        case Settings::Action::Unknown:
            break;
        }
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
            addAction(act);
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
