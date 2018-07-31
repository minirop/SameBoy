#include "gameboyworker.h"
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QApplication>

extern "C" {
#include <Core/gb.h>
}

constexpr int SCREEN_WIDTH = 160;
constexpr int SCREEN_HEIGHT = 144;

// mandatory(?) because a lambda-to-function-pointer must not capture.
static uint32_t pixel_buffer_1[SCREEN_WIDTH*SCREEN_HEIGHT];
static GameBoyWorker * gbWorker = nullptr;

GameBoyWorker::GameBoyWorker()
{
	gbWorker = this;
}

void GameBoyWorker::run()
{
    GB_gameboy_t gb;
	GB_init(&gb, GB_MODEL_CGB_E);
	GB_set_pixels_output(&gb, pixel_buffer_1);
	GB_set_vblank_callback(&gb, (GB_vblank_callback_t) [](GB_gameboy_t *) {
		QImage image((const unsigned char*)pixel_buffer_1, SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_RGB32);
		QPixmap pixmap = QPixmap::fromImage(image);
		gbWorker->render(pixmap);
	});
	GB_set_rgb_encode_callback(&gb, [](GB_gameboy_t *gb, uint8_t r, uint8_t g, uint8_t b) { return qRgb(r, g, b); });

	GB_load_boot_rom(&gb, "cgb_boot.bin");
	GB_load_rom(&gb, "Shantae (U) [C][!].gbc");

	running = true;
	while (running)
	{
		GB_run(&gb);
		QApplication::processEvents();
	}
}

void GameBoyWorker::render(QPixmap pixmap)
{
	rendered(pixmap);
}

void GameBoyWorker::stop()
{
	running = false;
}
