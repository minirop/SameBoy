#include "memorytable.h"

#include <algorithm>
#include <array>
#include <cctype>

#include <QScrollBar>
#include <QPainter>
#include <QDebug>

#define GB_INTERNAL

extern "C" {
#include "Core/gb.h"
}

class GBMemoryTablePrivate
{
public:
    GBMemoryTablePrivate(QWidget *parent);
    QString memoryLabel(uint16_t addr) const;

    QFont font;
    QFontMetrics metrics;

    QVector<uint8_t> visibleRegion;
    uint16_t startAddress = 0;

    uint8_t romBank = 0;
    uint8_t romBankCount = 0;
    uint8_t vramBank = 0;
    uint8_t vramBankCount = 0;
    uint8_t ramBank = 0;
    uint8_t ramBankCount = 0;
    uint8_t sramBank = 0;
    uint8_t sramBankCount = 0;

    GBMemoryTable::MemoryType selectedType = GBMemoryTable::All;
    uint8_t selectedBank = 0;
};

GBMemoryTablePrivate::GBMemoryTablePrivate(QWidget *parent)
    : font(QFont("monospace", 8), parent),
    metrics(font, parent)
{
}

template <typename T>
struct RegDesc {
    T offset;
    const char *name;
};

std::array<RegDesc<uint16_t>, 12> MEM_MAP = {{
    { 0x0000, "ROM0" },
    { 0x4000, "ROM0" },
    { 0x8000, "VRAM" },
    { 0xA000, "SRAM" },
    { 0xC000, "WRA0" },
    { 0xD000, "WRA0" },
    { 0xE000, "ECHO" },
    { 0xF000, "ECHO" },
    { 0xFE00, "OAM" },
    { 0xFEA0, "" },
    { 0xFF00, "I/O" },
    { 0xFF80, "HRAM" }
}};

static QString FormatAddressLabel(const char *prefix, uint8_t bank) {
    auto bankstr = QString::asprintf("%X", bank);
    return QString(prefix).left(4 - bankstr.length()) + bankstr;
}

QString GBMemoryTablePrivate::memoryLabel(uint16_t addr) const {
    for (auto iter = MEM_MAP.rbegin(); iter != MEM_MAP.rend(); iter++) {
        if (addr >= iter->offset) {
            if (iter->offset == 0x4000 && romBankCount > 0) {
                return FormatAddressLabel("ROM", romBank);
            } else if (iter->offset == 0x8000 && vramBankCount > 0) {
                return FormatAddressLabel("VRA", vramBank);
            } else if (iter->offset == 0xA000 && sramBankCount > 0) {
                return FormatAddressLabel("SRA", sramBank);
            } else if (iter->offset == 0xD000 && ramBankCount > 0) {
                return FormatAddressLabel("WRA", ramBank);
            } else if (iter->offset == 0xF000 && ramBankCount > 0) {
                return FormatAddressLabel("ECH", ramBank);
            }
            return iter->name;
        }
    }
    return "";
}

GBMemoryTable::GBMemoryTable(QWidget *parent)
    : QAbstractScrollArea(parent)
{
    priv.reset(new GBMemoryTablePrivate(viewport()));
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    resizeEvent(nullptr);
}

GBMemoryTable::~GBMemoryTable()
{
}

QSize GBMemoryTable::minimumSizeHint() const
{
    auto cw = priv->metrics.width("0");
    return { cw * (4 + 4 + 0x10 + 0x20 + 3) + 6 + verticalScrollBar()->size().width(), priv->metrics.height() };
}

uint8_t GBMemoryTable::bankCount(MemoryType type) const
{
    switch (type) {
    case ROM:
        return priv->romBankCount;
    case VRAM:
        return priv->vramBankCount;
    case SRAM:
        return priv->sramBankCount;
    case WRAM:
        return priv->ramBankCount;
    default:
        return 0;
    }
}

const static std::array<int, 5> REGION_SIZES = {0x10000, 0x4000, 0x2000, 0x2000, 0x1000};

void GBMemoryTable::updateMemory(GB_gameboy_t *gb)
{
    if (priv->selectedType == All) {
        if (gb->rom_size <= 0x8000) {
            priv->romBankCount = 0;
        } else {
            priv->romBankCount = gb->rom_size >> 14;
        }
        priv->romBank = gb->mbc_rom_bank;
        priv->sramBankCount = gb->mbc_ram_size >> 13;
        priv->sramBank = gb->mbc_ram_bank;
        if (GB_is_cgb(gb)) {
            priv->vramBankCount = 2;
            priv->vramBank = gb->cgb_vram_bank;
            priv->ramBankCount = 8;
            priv->ramBank = gb->cgb_ram_bank;
        } else {
            priv->vramBankCount = 0;
            priv->vramBank = 0;
            priv->ramBankCount = 0;
            priv->ramBank = 0;
        }
    }

    priv->visibleRegion.clear();
    priv->startAddress = verticalScrollBar()->value() << 4;

    if (priv->selectedType == All || priv->selectedBank < bankCount(priv->selectedType)) {
        int h = size().height();
        int ch = priv->metrics.height();
        int vh = (h - 4) / ch + 1;
        uint32_t maxAddress = REGION_SIZES[priv->selectedType];
        uint32_t endAddress = std::min(uint32_t(priv->startAddress + (vh << 4)), maxAddress);

        priv->visibleRegion.reserve(endAddress - priv->startAddress);
        for (uint32_t addr = priv->startAddress; addr < endAddress; addr++) {
            uint8_t value;
            switch (priv->selectedType) {
            case All:
                value = GB_read_memory(gb, addr);
                break;
            case ROM:
                if (priv->selectedBank == 0) {
                    value = GB_read_memory(gb, addr);
                } else {
                    value = gb->rom[addr + priv->selectedBank * maxAddress];
                }
                break;
            case VRAM:
                value = gb->vram[addr + priv->selectedBank * maxAddress];
                break;
            case SRAM:
                value = gb->mbc_ram[addr + priv->selectedBank * maxAddress];
                break;
            case WRAM:
                value = gb->ram[addr + priv->selectedBank * maxAddress];
                break;
            default:
                value = 0xFF;
                break;
            }
            priv->visibleRegion.push_back(value);
        }
    }

    viewport()->update();
}

void GBMemoryTable::setRegion(MemoryType type, uint8_t bank)
{
    priv->selectedType = type;
    priv->selectedBank = bank;
    resizeEvent(nullptr);
}


void GBMemoryTable::resizeEvent(QResizeEvent *)
{
    int ch = priv->metrics.height();
    int vh = (size().height() - 4) / ch;
    int mh = REGION_SIZES[priv->selectedType] / 0x10 - vh;
    verticalScrollBar()->setRange(0, mh);
    viewport()->update();
}

void GBMemoryTable::paintEvent(QPaintEvent *)
{
    QPainter paint(viewport());

    paint.setFont(priv->font);
    uint32_t addr = verticalScrollBar()->value() << 4;
    int cw = priv->metrics.width("0");
    int ch = priv->metrics.height();
    int h = size().height() + ch;
    int x = 0;
    int y = ch - 2;
    int offset = int(addr) - int(priv->startAddress);
    std::string label;
    if (priv->selectedType != All) {
        const static std::array<const char *, 4> REGION_NAMES = {"ROM", "VRAM", "SRAM", "WRAM"};
        label = FormatAddressLabel(REGION_NAMES[priv->selectedType - 1], priv->selectedBank).toStdString();
    }
    while (y < h) {
        if (addr >= REGION_SIZES[priv->selectedType]) {
            break;
        }
        if (x == 0) {
            if (priv->selectedType == All) {
                label = priv->memoryLabel(addr).toStdString();
            }
            paint.drawText(2, y, QString::asprintf("%4s:%04X", label.c_str(), addr));
        }
        if (offset >= 0 && offset < priv->visibleRegion.length()) {
            uint8_t value = priv->visibleRegion[offset];
            paint.drawText(2 + (x * 2 + 10) * cw, y, QString::asprintf("%02X", value));
            char c = std::isprint(value) ? value : '.';
            paint.drawText(2 + (x + 10 + 0x20 + 1) * cw, y, QString::asprintf("%c", c));
        }

        x++;
        offset++;
        if (x == 0x10) {
            x = 0;
            y += ch;
            addr += 0x10;
        }
    }
}
