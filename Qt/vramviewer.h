#ifndef VRAMVIEWER_H
#define VRAMVIEWER_H

#include <QTabWidget>

extern "C" {
#include <Core/gb.h>
}

#include "details.h"

class VramViewer : public QTabWidget
{
    Q_OBJECT

public:
    VramViewer(QWidget * parent = nullptr);

public slots:
    void updateMemory(GB_gameboy_t * gb);

protected:
    bool eventFilter(QObject * obj, QEvent * event);

private:
    GB_map_type_t mapType = GB_MAP_AUTO;
    GB_tileset_type_t tilesetType = GB_TILESET_AUTO;

    QLabel backgroundLabel;
    QLabel backgroundTileLabel;
    QRect tileRect;

    Details * detailsBox = nullptr;
};

#endif
