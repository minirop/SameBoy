#include "settings.h"
#include <QKeySequence>

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

	for (GB_key_t key = GB_KEY_RIGHT; key < GB_KEY_MAX; (*reinterpret_cast<int*>(&key))++)
	{
		keys[key] = loadKey(key);
	}
}

Settings::~Settings()
{
	int i = 1;
	for (auto & filename : recentRoms)
	{
		settings.setValue(QString("recent/rom_%1").arg(i), filename);
	}

	for (GB_key_t key = GB_KEY_RIGHT; key < GB_KEY_MAX; (*reinterpret_cast<int*>(&key))++)
	{
		int keycode = keys[key];
		QString path = "keys/" + getKeyName(key);

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
}

int Settings::getKey(GB_key_t key)
{
	if (keys.contains(key))
		return keys[key];
	return Qt::Key_unknown;
}

int Settings::loadKey(GB_key_t key)
{
	QString path = "keys/" + getKeyName(key);
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

QString Settings::getKeyName(GB_key_t key)
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
