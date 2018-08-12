#include "vramviewer.h"
#include "globals.h"
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QGroupBox>
#include <QGridLayout>
#include <QMouseEvent>
#include <QLineEdit>
#include <QDebug>

VramViewer::VramViewer()
	: tileRect { 0, 0, 8, 8 }
{
	setWindowTitle("QameBoy VRAM Viewer");
    setFixedSize(460, 320);

	{
		auto bgMap = new QWidget;
		auto layout = new QGridLayout(bgMap);
		backgroundLabel = new QLabel;
        backgroundLabel->setFixedSize(BUFFER_WIDTH, BUFFER_HEIGHT);
		backgroundTileLabel = new QLabel;
        backgroundTileLabel->setFixedSize(TILE_WIDTH * 8, TILE_HEIGHT * 8);
        auto groupBox = new QGroupBox("Details");

        auto gridLayout = new QGridLayout(groupBox);
        xLineEdit = new QLineEdit;
        xLineEdit->setReadOnly(true);
        gridLayout->addWidget(xLineEdit, 0, 0);
        gridLayout->addWidget(new QLabel("X"), 0, 1);
        tileIdLineEdit = new QLineEdit;
        tileIdLineEdit->setReadOnly(true);
        gridLayout->addWidget(tileIdLineEdit, 0, 2);
        gridLayout->addWidget(new QLabel("Tile No."), 0, 3);

        yLineEdit = new QLineEdit;
        yLineEdit->setReadOnly(true);
        gridLayout->addWidget(yLineEdit, 1, 0);
        gridLayout->addWidget(new QLabel("Y"), 1, 1);
        attributeLineEdit = new QLineEdit;
        attributeLineEdit->setReadOnly(true);
        gridLayout->addWidget(attributeLineEdit, 1, 2);
        gridLayout->addWidget(new QLabel("Attribute"), 1, 3);

		layout->addWidget(backgroundLabel, 0, 0, 2, 1, Qt::AlignLeft | Qt::AlignTop);
		layout->addWidget(backgroundTileLabel, 0, 1, Qt::AlignLeft | Qt::AlignTop);
		layout->addWidget(groupBox, 1, 1, Qt::AlignLeft | Qt::AlignTop);

        layout->setRowStretch(1, 1);

		addTab(bgMap, "BG Map");

		backgroundLabel->installEventFilter(this);
		backgroundLabel->setMouseTracking(true);
	}
}

void VramViewer::updateMemory(GB_gameboy_t * gb)
{
	if (!isVisible()) return;

	uint32_t screen[BUFFER_WIDTH*BUFFER_HEIGHT];
	GB_draw_tilemap(gb, screen, GB_PALETTE_AUTO, 0, GB_MAP_AUTO, GB_TILESET_AUTO);

	int x = GB_read_memory(gb, 0xFF00 + GB_IO_SCX);
	int y = GB_read_memory(gb, 0xFF00 + GB_IO_SCY);

	QImage image(reinterpret_cast<const unsigned char*>(screen), BUFFER_WIDTH, BUFFER_HEIGHT, QImage::Format_ARGB32);
	auto pixmap = QPixmap::fromImage(image);
	QPainter p;
	p.begin(&pixmap);
	p.setCompositionMode(QPainter::CompositionMode_Difference);
	p.setPen(QPen(Qt::white, 2));
	p.drawRect(x, y, SCREEN_WIDTH, SCREEN_HEIGHT);
	p.end();
	backgroundLabel->setPixmap(pixmap);

	auto bgTile = QPixmap::fromImage(image.copy(tileRect)).scaled(64, 64);
	backgroundTileLabel->setPixmap(bgTile);

    auto px = tileRect.x() / TILE_WIDTH;
    auto py = tileRect.y() / TILE_HEIGHT;
    auto vram = static_cast<uint8_t*>(GB_get_direct_access(gb, GB_DIRECT_ACCESS_VRAM, nullptr, nullptr));
    auto tile = vram[0x1800 + px + py * (BUFFER_WIDTH / TILE_WIDTH)];
    auto attribute = vram[0x1800 + px + py * (BUFFER_WIDTH / TILE_WIDTH) + 0x2000];

    xLineEdit->setText(QString::number(tileRect.x() / TILE_WIDTH).rightJustified(2, '0'));
    tileIdLineEdit->setText(QString::number(tile, 16).rightJustified(2, '0'));
    yLineEdit->setText(QString::number(tileRect.y() / TILE_HEIGHT).rightJustified(2, '0'));
    attributeLineEdit->setText(QString::number(attribute, 16).rightJustified(2, '0'));
}

bool VramViewer::eventFilter(QObject * obj, QEvent * event)
{
	if (obj == backgroundLabel)
	{
		if (event->type() == QEvent::MouseMove)
		{
			auto mouseEvent = static_cast<QMouseEvent *>(event);
			auto pos = mouseEvent->pos();
			auto x = pos.x();
			auto y = pos.y();
            if (x >= 0 && x < BUFFER_WIDTH && y >= 0 && y < BUFFER_HEIGHT)
			{
                tileRect.moveTo(pos.x() / TILE_WIDTH * TILE_WIDTH, pos.y() / TILE_HEIGHT * TILE_HEIGHT);
			}
		}
	}

	return QWidget::eventFilter(obj, event);
}
