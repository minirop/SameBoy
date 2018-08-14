#include "vramviewer.h"
#include "globals.h"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QGridLayout>
#include <QMouseEvent>
#include <QDebug>
#include "details.h"

VramViewer::VramViewer(QWidget * parent)
    : QTabWidget(parent)
    , tileRect { 0, 0, 8, 8 }
{
    setWindowTitle("QameBoy VRAM Viewer");
    setFixedSize(500, 320);
    setWindowFlags(Qt::Dialog);

    {
        auto bgMap = new QWidget;
        auto layout = new QGridLayout(bgMap);
        backgroundLabel.setFixedSize(BUFFER_WIDTH, BUFFER_HEIGHT);
        backgroundTileLabel.setFixedSize(TILE_WIDTH * 8, TILE_HEIGHT * 8);
        detailsBox = new Details(false);

        layout->addWidget(&backgroundLabel, 0, 0, 2, 1, Qt::AlignLeft | Qt::AlignTop);
        layout->addWidget(&backgroundTileLabel, 0, 1, Qt::AlignLeft | Qt::AlignTop);
        layout->addWidget(detailsBox, 1, 1, Qt::AlignLeft | Qt::AlignTop);

        layout->setRowStretch(1, 1);
        layout->setColumnStretch(1, 1);

        addTab(bgMap, "BG Map");

        backgroundLabel.installEventFilter(this);
        backgroundLabel.setMouseTracking(true);
    }
}

void VramViewer::updateMemory(GB_gameboy_t * gb)
{
    if (!isVisible()) return;

    int x = GB_read_memory(gb, 0xFF00 + GB_IO_SCX);
    int y = GB_read_memory(gb, 0xFF00 + GB_IO_SCY);

    uint32_t screen[BUFFER_WIDTH*BUFFER_HEIGHT];
    GB_draw_tilemap(gb, screen, GB_PALETTE_AUTO, 0, mapType, tilesetType);

    QImage image(reinterpret_cast<const unsigned char*>(screen), BUFFER_WIDTH, BUFFER_HEIGHT, QImage::Format_ARGB32);
    auto pixmap = QPixmap::fromImage(image);
    QPainter p;
    p.begin(&pixmap);
    p.setCompositionMode(QPainter::CompositionMode_Difference);
    p.setPen(QPen(Qt::white, 2));
    p.drawRect(x, y, SCREEN_WIDTH, SCREEN_HEIGHT);
    p.end();
    backgroundLabel.setPixmap(pixmap);

    auto bgTile = QPixmap::fromImage(image.copy(tileRect)).scaled(64, 64);
    backgroundTileLabel.setPixmap(bgTile);

    auto px = tileRect.x() / TILE_WIDTH;
    auto py = tileRect.y() / TILE_HEIGHT;
    auto vram = static_cast<uint8_t*>(GB_get_direct_access(gb, GB_DIRECT_ACCESS_VRAM, nullptr, nullptr));
    auto tilePos = 0x1800 + px + py * TILE_COUNT_WIDTH;
    auto attrPos = tilePos + 0x2000;
    auto tile = vram[tilePos];
    auto attribute = vram[attrPos];

    detailsBox->setTileData(px, py, tile, attribute);
}

bool VramViewer::eventFilter(QObject * obj, QEvent * event)
{
    if (obj == &backgroundLabel)
    {
        if (event->type() == QEvent::MouseMove)
        {
            auto mouseEvent = static_cast<QMouseEvent *>(event);
            auto pos = mouseEvent->pos();
            auto x = pos.x();
            auto y = pos.y();
            if (x >= 0 && x < BUFFER_WIDTH && y >= 0 && y < BUFFER_HEIGHT)
            {
                tileRect.moveTo(x & 0xFFFFFFF8, y & 0xFFFFFFF8);
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}
