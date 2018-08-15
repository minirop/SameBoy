#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVector>
#include <QString>

extern "C" {
#include <Core/joypad.h>
}

class Settings
{
public:
    enum class Action {
        Turbo,
        Unknown
    };

    Settings();
    ~Settings();

    QVector<QString> getRecentRoms();
    void addRecentRom(QString filename);

    int getGbKey(GB_key_t key);
    GB_key_t getGbKey(int key);
    int loadGbKey(GB_key_t key);
    QString getGbKeyName(GB_key_t key);
    int getActionKey(Action action);
    Action getAction(int key);

private:
    QVector<QString> recentRoms;
    QMap<GB_key_t, int> gbKeys;
    QMap<Action, int> actionKeys;
    QSettings settings;
};

#endif
