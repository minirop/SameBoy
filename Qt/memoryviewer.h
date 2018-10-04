#ifndef MEMORYVIEWER_H
#define MEMORYVIEWER_H

#include <array>

#include <QWidget>

extern "C" {
#include "Core/gb_struct_def.h"
}

class GBMemoryTable;
class QComboBox;
class QSpinBox;

class MemoryViewer : public QWidget
{
    Q_OBJECT

public:
    MemoryViewer(QWidget * parent = nullptr);

public slots:
    void updateMemory(GB_gameboy_t *gb);

/*
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    */

private:
    void typeChanged(int index);
    void bankChanged(int index);

    GBMemoryTable *memoryTable;
    QComboBox *memoryType;
    QSpinBox *memoryBank;

    std::array<uint8_t, 4> selectedBanks;
};

#endif
