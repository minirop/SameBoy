#include "vramviewer.h"
#include "globals.h"
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

VramViewer::VramViewer()
{
	setWindowTitle("QameBoy VRAM Viewer");

	backgroundLabel = new QLabel;
	addTab(backgroundLabel, "BG Map");

	resize(300, 300);
}

void VramViewer::updateMemory(GB_gameboy_t * gb)
{
	if (!isVisible()) return;

	uint32_t screen[BUFFER_WIDTH*BUFFER_HEIGHT];
	GB_draw_tilemap(gb, screen, GB_PALETTE_AUTO, 0, GB_MAP_AUTO, GB_TILESET_AUTO);

	int x = GB_read_memory(gb, 0xFF00 + GB_IO_SCX);
	int y = GB_read_memory(gb, 0xFF00 + GB_IO_SCY);

	QImage image(reinterpret_cast<const unsigned char*>(screen), BUFFER_WIDTH, BUFFER_HEIGHT, QImage::Format_ARGB32);
	QPixmap pixmap = QPixmap::fromImage(image);
	QPainter p;
	p.begin(&pixmap);
	p.drawRect(x, y, SCREEN_WIDTH, SCREEN_HEIGHT);
	p.end();
	backgroundLabel->setPixmap(pixmap);
}
