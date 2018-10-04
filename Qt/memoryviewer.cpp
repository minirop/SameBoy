#define GB_INTERNAL

#include "memoryviewer.h"
#include "memorytable.h"
#include "globals.h"

extern "C" {
#include "Core/gb.h"
}

#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

MemoryViewer::MemoryViewer(QWidget * parent)
  : QWidget(parent)
{
    setWindowTitle(tr("QameBoy Debugger"));
    setWindowFlags(Qt::Dialog);

    selectedBanks.fill(0);

    memoryTable = new GBMemoryTable;

    {
        auto vlayout = new QVBoxLayout;
        vlayout->setSpacing(6);
        {
            auto header = new QWidget;
            auto headerlayout = new QHBoxLayout;
            header->setLayout(headerlayout);
            {
                auto label = new QLabel(tr("Region"));
                headerlayout->addWidget(label);
                memoryType = new QComboBox;
                memoryType->addItems({
                    tr("Entire Space"),
                    tr("ROM"),
                    tr("Video RAM"),
                    tr("Cartridge RAM"),
                    tr("RAM")
                });
                QObject::connect(
                    memoryType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                    this, &MemoryViewer::typeChanged);
                memoryType->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                headerlayout->addWidget(memoryType);
            }
            {
                auto label = new QLabel(tr("Bank"));
                headerlayout->addWidget(label);
                memoryBank = new QSpinBox;
                QObject::connect(
                    memoryBank, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                    this, &MemoryViewer::bankChanged);
                memoryBank->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                memoryBank->setDisabled(true);
                headerlayout->addWidget(memoryBank);
            }
            {
                auto spacer = new QWidget;
                spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
                headerlayout->addWidget(spacer);
            }
            vlayout->addWidget(header);
        }

        vlayout->addWidget(memoryTable);
        setLayout(vlayout);
    }
}

void MemoryViewer::updateMemory(GB_gameboy_t * gb)
{
    if (!isVisible()) return;
    memoryTable->updateMemory(gb);
}

void MemoryViewer::typeChanged(int index) {
    auto type = static_cast<GBMemoryTable::MemoryType>(index);
    if (index == 0) {
        memoryBank->setValue(0);
        memoryBank->setDisabled(true);
        memoryTable->setRegion(GBMemoryTable::All, 0);
    } else {
        uint8_t bank = selectedBanks[index - 1];
        memoryBank->setRange(0, memoryTable->bankCount(type) - 1);
        memoryBank->setValue(bank);
        memoryBank->setDisabled(false);
        memoryTable->setRegion(type, bank);
    }
}

void MemoryViewer::bankChanged(int bank)
{
    int index = memoryType->currentIndex();
    auto type = static_cast<GBMemoryTable::MemoryType>(index);
    memoryTable->setRegion(type, bank);
    if (type != GBMemoryTable::All) {
      selectedBanks[index - 1] = bank;
    }
}
