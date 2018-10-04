#ifndef MEMORYVIEWS_H
#define MEMORYVIEWS_H

#include <QAbstractScrollArea>
#include <QScopedPointer>

extern "C" {
#include "Core/gb_struct_def.h"
}


class GBMemoryTablePrivate;

class GBMemoryTable : public QAbstractScrollArea
{
    Q_OBJECT

public:
    enum MemoryType {
        All,
        ROM,
        VRAM,
        SRAM,
        WRAM
    };

    GBMemoryTable(QWidget *parent = 0);
    virtual ~GBMemoryTable();

    virtual QSize minimumSizeHint() const override;
    uint8_t bankCount(MemoryType type) const;

public slots:
    void updateMemory(GB_gameboy_t *gb);
    void setRegion(MemoryType type, uint8_t bank);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    QScopedPointer<GBMemoryTablePrivate> priv;
};

#endif
