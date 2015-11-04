#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "threadmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
	void on_startButton_clicked();
	void on_stopButton_clicked();

private:
    Ui::MainWindow *ui;
	ThreadManager m_manager;
};

#endif // MAINWINDOW_H
