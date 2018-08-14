#include "details.h"
#include "globals.h"
#include <QGridLayout>

Details::Details(bool sprite)
    : QGroupBox("Details")
{
    auto gridLayout = new QGridLayout(this);
    xLineEdit.setReadOnly(true);
    gridLayout->addWidget(&xLineEdit, 0, 0);
    gridLayout->addWidget(new QLabel("X"), 0, 1);
    tileIdLineEdit.setReadOnly(true);
    gridLayout->addWidget(&tileIdLineEdit, 0, 2);
    gridLayout->addWidget(new QLabel("Tile No."), 0, 3);

    yLineEdit.setReadOnly(true);
    gridLayout->addWidget(&yLineEdit, 1, 0);
    gridLayout->addWidget(new QLabel("Y"), 1, 1);
    attributeLineEdit.setReadOnly(true);
    gridLayout->addWidget(&attributeLineEdit, 1, 2);
    gridLayout->addWidget(new QLabel("Attribute"), 1, 3);

    mapAddress.setReadOnly(true);
    gridLayout->addWidget(&mapAddress, 2, 0, 1, 2);
    gridLayout->addWidget(new QLabel("Map Address"), 2, 2, 1, 2);

    tileAddress.setReadOnly(true);
    gridLayout->addWidget(&tileAddress, 3, 0, 1, 2);
    gridLayout->addWidget(new QLabel("Tile Address"), 3, 2, 1, 2);

    xFlipCheckBox.setText("X-flip");
    xFlipCheckBox.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    xFlipCheckBox.setFocusPolicy(Qt::NoFocus);
    gridLayout->addWidget(&xFlipCheckBox, 4, 0, 1, 2);

    backgroundLineEdit.setReadOnly(true);
    gridLayout->addWidget(&backgroundLineEdit, 4, 2);
    gridLayout->addWidget(new QLabel("Palette"), 4, 3);

    yFlipCheckBox.setText("Y-flip");
    yFlipCheckBox.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    yFlipCheckBox.setFocusPolicy(Qt::NoFocus);
    gridLayout->addWidget(&yFlipCheckBox, 5, 0, 1, 2);

    priorityCheckBox.setText("Y-flip");
    priorityCheckBox.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    priorityCheckBox.setFocusPolicy(Qt::NoFocus);
    gridLayout->addWidget(&priorityCheckBox, 5, 2, 1, 2);

    if (!sprite)
    {
        xFlipCheckBox.setVisible(false);
        yFlipCheckBox.setVisible(false);
        priorityCheckBox.setVisible(false);
    }
}

void Details::setTileData(int x, int y, int tile, int attributes)
{
    auto tileAddr = tile * 0x10;
    auto memoryBank = (attributes & 0x08) >> 3;
    auto bgId = (attributes & 0x07);

    xLineEdit.setText(QString("%1").arg(x, 2, 16, QLatin1Char('0')));
    tileIdLineEdit.setText(QString("%1").arg(tile, 2, 16, QLatin1Char('0')));
    yLineEdit.setText(QString("%1").arg(y, 2, 16, QLatin1Char('0')));
    attributeLineEdit.setText(QString("%1").arg(attributes, 2, 16, QLatin1Char('0')));

    mapAddress.setText(QString("%1").arg(mapBase + x + y * TILE_COUNT_WIDTH, 4, 16, QLatin1Char('0')));
    tileAddress.setText(QString("%1:%2").arg(memoryBank).arg(0x8000 + tileAddr, 4, 16, QLatin1Char('0')));
    backgroundLineEdit.setText(QString("BG %1").arg(bgId));
}
