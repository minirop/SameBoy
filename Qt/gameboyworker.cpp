#include "gameboyworker.h"
#include "globals.h"
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QApplication>

// mandatory because a lambda-to-function-pointer must not capture.
static uint32_t pixel_buffer_1[SCREEN_WIDTH*SCREEN_HEIGHT];

GameBoyWorker::GameBoyWorker()
{
    GB_init(&gb, GB_MODEL_CGB_E);
    GB_set_pixels_output(&gb, pixel_buffer_1);
    GB_set_user_data(&gb, this);
    GB_set_vblank_callback(&gb, (GB_vblank_callback_t) [](GB_gameboy_t *  gb) {
        auto gbWorker = static_cast<GameBoyWorker*>(GB_get_user_data(gb));
        QImage image(reinterpret_cast<const unsigned char*>(pixel_buffer_1), SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_RGB32);
        QPixmap pixmap = QPixmap::fromImage(image);
        gbWorker->render(pixmap);
        gbWorker->grabState();
    });
    GB_set_rgb_encode_callback(&gb, [](GB_gameboy_t *gb, uint8_t r, uint8_t g, uint8_t b) { return qRgb(r, g, b); });

    auto romFile = QString("%1/cgb_boot.bin").arg(QApplication::applicationDirPath());
    GB_load_boot_rom(&gb, qPrintable(romFile));
}

GameBoyWorker::~GameBoyWorker()
{
    GB_free(&gb);
}

void GameBoyWorker::loadRom(QString filename)
{
    GB_load_rom(&gb, qPrintable(filename));
}

void GameBoyWorker::run()
{
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

void GameBoyWorker::keyEvent(GB_key_t index, bool pressed)
{
    GB_set_key_state(&gb, index, pressed);
}

void GameBoyWorker::reset()
{
    GB_reset(&gb);
}

void GameBoyWorker::setTurbo(bool enabled)
{
    GB_set_turbo_mode(&gb, enabled, enabled);
}

void GameBoyWorker::grabState()
{
    emit gbState(&gb);
}
