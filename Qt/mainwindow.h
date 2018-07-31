#ifndef MAINWINDOW_H	
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QPixmap>

class QPaintEvent;
class MainWindow : public QWidget
{
	Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

public slots:
	void updateCache(QPixmap pixmap);

protected:
    void paintEvent(QPaintEvent * event);

private:
	QThread thread;
	QPixmap cache;
};

#endif
