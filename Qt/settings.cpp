#include "settings.h"
#include <QKeySequence>
#include <type_traits>

Settings::Settings()
    : settings { "qameboy.ini", QSettings::IniFormat }
{
    for (int i = 1; i < 11; i++)
    {
        QString filename = settings.value(QString("recent/rom_%1").arg(i)).toString();
        if (filename.isEmpty())
        {
            break;
        }
        recentRoms.push_back(filename);
    }

    for (int key = GB_KEY_RIGHT; key < GB_KEY_MAX; key++)
    {
        gbKeys[static_cast<GB_key_t>(key)] = loadGbKey(static_cast<GB_key_t>(key));
    }

    actionKeys[Action::Turbo] = Qt::Key_M;
}

Settings::~Settings()
{
    int i = 1;
    for (auto & filename : recentRoms)
    {
        settings.setValue(QString("recent/rom_%1").arg(i), filename);
    }

    for (int key = GB_KEY_RIGHT; key < GB_KEY_MAX; key++)
    {
        int keycode = gbKeys[static_cast<GB_key_t>(key)];
        QString path = "keys/" + getGbKeyName(static_cast<GB_key_t>(key));

        settings.setValue(path, QKeySequence(keycode).toString());
    }
}

QVector<QString> Settings::getRecentRoms()
{
    return recentRoms;
}

void Settings::addRecentRom(QString filename)
{
    recentRoms.removeOne(filename);
    recentRoms.push_front(filename);

    if (recentRoms.size() > 10)
    {
        recentRoms = recentRoms.mid(0, 10);
    }
}

int Settings::getGbKey(GB_key_t key)
{
    if (gbKeys.contains(key))
    {
        return gbKeys[key];
    }
    return Qt::Key_unknown;
}

GB_key_t Settings::getGbKey(int key)
{
    return gbKeys.key(key, GB_KEY_MAX);
}

int Settings::loadGbKey(GB_key_t key)
{
    QString path = "keys/" + getGbKeyName(key);
    int defaultValue = Qt::Key_unknown;
    switch (key)
    {
    case GB_KEY_RIGHT:
        defaultValue = Qt::Key_Right;
        break;
    case GB_KEY_LEFT:
        defaultValue = Qt::Key_Left;
        break;
    case GB_KEY_UP:
        defaultValue = Qt::Key_Up;
        break;
    case GB_KEY_DOWN:
        defaultValue = Qt::Key_Down;
        break;
    case GB_KEY_A:
        defaultValue = Qt::Key_Space;
        break;
    case GB_KEY_B:
        defaultValue = Qt::Key_D;
        break;
    case GB_KEY_SELECT:
        defaultValue = Qt::Key_Backspace;
        break;
    case GB_KEY_START:
        defaultValue = Qt::Key_Return;
        break;
    case GB_KEY_MAX:
        break;
    }

    QString value = settings.value(path).toString();
    if (value.size())
    {
        defaultValue = QKeySequence::fromString(value)[0];
    }

    return defaultValue;
}

QString Settings::getGbKeyName(GB_key_t key)
{
    switch (key)
    {
    case GB_KEY_RIGHT:
        return "Right";
    case GB_KEY_LEFT:
        return "Left";
    case GB_KEY_UP:
        return "Up";
    case GB_KEY_DOWN:
        return "Down";
    case GB_KEY_A:
        return "A";
    case GB_KEY_B:
        return "B";
    case GB_KEY_SELECT:
        return "Select";
    case GB_KEY_START:
        return "Start";
    case GB_KEY_MAX:
        break;
    }

    return {};
}

int Settings::getActionKey(Settings::Action action)
{
    switch (action)
    {
    case Action::Turbo:
        return Qt::Key_M;
    case Action::Unknown:
        break;
    }
    return Qt::Key_unknown;
}

Settings::Action Settings::getAction(int key)
{
    return actionKeys.key(key, Action::Unknown);
}
