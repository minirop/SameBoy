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
	Settings();
	~Settings();

	QVector<QString> getRecentRoms();
	void addRecentRom(QString filename);

	int getKey(GB_key_t key);
	int loadKey(GB_key_t key);
	QString getKeyName(GB_key_t key);

private:
	QVector<QString> recentRoms;
	QMap<GB_key_t, int> keys;
	QSettings settings;
};

#endif
