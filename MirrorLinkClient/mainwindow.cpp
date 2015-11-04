#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
	QString t = ui->locLineEdit->text();
	QUrl url(t);
	m_manager.startMirrorLinkClient(url.host(), url.port(), url.path());
}

void MainWindow::on_stopButton_clicked()
{
	m_manager.stopMirrorLinkClient();
}
