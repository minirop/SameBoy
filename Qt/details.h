#ifndef DETAILS_H
#define DETAILS_H

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>

class Details : public QGroupBox
{
public:
    Details(bool sprite);

    void setTileData(int x, int y, int tile, int attributes);

private:
    QLineEdit xLineEdit;
    QLineEdit tileIdLineEdit;
    QLineEdit yLineEdit;
    QLineEdit attributeLineEdit;
    QLineEdit mapAddress;
    QLineEdit tileAddress;
    QCheckBox xFlipCheckBox;
    QLineEdit backgroundLineEdit;
    QCheckBox yFlipCheckBox;
    QCheckBox priorityCheckBox;
    int mapBase = 0x9800;
};

#endif
