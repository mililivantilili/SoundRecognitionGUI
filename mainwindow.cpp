#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkAccessManager>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QtNetwork>
#include <QByteArray>
#include <QObject>
#include <QtCharts>
#include <QLogValueAxis>
#include <QLineSeries>
#include <QValueAxis>
#include <QChartView>
#include <QString>
#include <QStringList>
#include <QThread>

#define SAMPLE_NUM 5000
//TCP server
#define TCP_PORT 51717
//'TCP server

//void setChart();
bool calculationDone = true;
bool firstGraphSet = true;
bool downloadError = false;

using namespace QtCharts;

//QLineSeries *series;
QChartView *chartView;
QChart *chart;
QValueAxis *axisX;
QValueAxis *axisY;

QThread cThread;

QNetworkReply *reply;

bool downloadingInProgress = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "1";
    lThread = new loadingThread(this);
    connect(lThread,SIGNAL(dataChanged(QLineSeries*)),this, SLOT(onDataChanged(QLineSeries*)));
    qDebug() << "2";

    chartView = new QChartView(this);
    ui->verticalLayout->addWidget(chartView);

    //series = new QLineSeries();

    chart = new QChart();

    chart->legend()->hide();

    axisX = new QValueAxis();
    axisX->setRange(0, (ui->xSlider->value()));
    //axisX->setRange((ui->xSlider->value())-((ui->xEdit->text().toInt())/2), (ui->xSlider->value())+((ui->xEdit->text().toInt())/2));
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Samples");
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QValueAxis();
    axisY->setRange((ui->ySlider->value())-((ui->yEdit->text().toInt())/2), (ui->ySlider->value())+((ui->yEdit->text().toInt())/2));
    axisY->setTitleText("Audio level");
    chart->addAxis(axisY, Qt::AlignLeft);

    chartView->setChart(chart);

    lThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(calculationDone)
    {
        calculationDone = false;
        ui->progressBar->setValue(0);
        ui->progressBar->setFormat("Downloading... %p%");
        QUrl url(ui->lineEditIP->text());   //funguje i pro http
        url.setUserName(ui->lineEditName->text());
        url.setPassword(ui->lineEditPassword->text());
        url.setPort(21);

        qDebug() << "3";
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        reply = manager->get(QNetworkRequest(url));

        qDebug() << "4";
        connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(requestError(QNetworkReply::NetworkError)));
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(onDownloadProgress(qint64,qint64)));
        qDebug() << "5";
        //cObject.setUrl(url);
        downloadingInProgress = 1;
    }

}

void MainWindow::setChart()
{
    axisX->setRange((ui->xSlider->value()), (ui->xSlider->value())+((ui->xEdit->text().toInt())));
    axisY->setRange((ui->ySlider->value())-((ui->yEdit->text().toInt())/2), (ui->ySlider->value())+((ui->yEdit->text().toInt())/2));
}

void MainWindow::on_ySlider_sliderMoved(int position)
{
    setChart();
}

void MainWindow::on_yEdit_editingFinished()
{
    setChart();
}

void MainWindow::on_xSlider_sliderMoved(int position)
{
    setChart();
}

void MainWindow::on_xEdit_editingFinished()
{
    setChart();
}

void MainWindow::onDataChanged(QLineSeries *lSeries)
{
    qDebug() << "on data changed";
    if (firstGraphSet)
    {
        firstGraphSet = false;
        chart->addSeries(lSeries);
        lSeries->attachAxis(axisX);
        lSeries->attachAxis(axisY);
        chartView->setChart(chart);

    }
    else
    {

        chart->removeSeries(lSeries);
        chart->addSeries(lSeries);
        lSeries->attachAxis(axisX);
        lSeries->attachAxis(axisY);
        //axisX->setRange(0, 5000);
    }

    calculationDone = true;

    //ui->label->setText(QString::number(intData[0]) + " " + QString::number(intData[1]);

}

void MainWindow::requestError(QNetworkReply::NetworkError err)
{
    downloadError = true;
    qDebug() << "7";
    qDebug() << "error" << err;
}

void MainWindow::requestFinished(QNetworkReply *reply)
{
    downloadingInProgress = 0;
    if(!downloadError)
    {
        ui->progressBar->setFormat("Success %p%");
        qDebug() << "request finished";
        QString stringReplay = reply->readAll();
        lThread->calculateGraph(stringReplay);
        lThread->calculate = true;
        reply->deleteLater();
    }
    else
    {
        ui->progressBar->setFormat("Download failed %p%");
        downloadError = false;
        calculationDone = true;
    }
}

void MainWindow::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qDebug() << "Download progress:" << bytesReceived << bytesTotal;
    if (bytesTotal != -1)
    {
        ui->progressBar->setValue((int)((float)bytesReceived/bytesTotal)*100);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (downloadingInProgress)
    {
        reply->close();
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    //https://stackoverflow.com/questions/20546750/qtcpsocket-reading-and-writing
    qDebug() << "Clicked";
    QUrl url(ui->lineEditIP->text());
    QTcpSocket *socket = new QTcpSocket(this);
    qDebug() << "Cetting connection";
    socket->connectToHost(url.host(),TCP_PORT);
    qDebug() << "Waiting to cennection";
    socket->waitForConnected(30000);
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << "Connected, writing";
        socket->write("StartRecording");
        socket->waitForBytesWritten(30000);
        socket->waitForReadyRead(30000);
        qDebug() << socket->readAll();
    }
    else
    {
         qDebug() << "Connection failed";
    }
    qDebug() << "Conection ended";
}

void MainWindow::on_pushButton_4_clicked()
{
    //https://stackoverflow.com/questions/20546750/qtcpsocket-reading-and-writing
    qDebug() << "Clicked";
    QUrl url(ui->lineEditIP->text());
    QTcpSocket *socket = new QTcpSocket(this);
    qDebug() << "Cetting connection";
    socket->connectToHost(url.host(),TCP_PORT);
    qDebug() << "Waiting to cennection";
    socket->waitForConnected(30000);
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << "Connected, writing";
        socket->write("StopRecording");
        socket->waitForBytesWritten(30000);
        socket->waitForReadyRead(30000);
        qDebug() << socket->readAll();
    }
    else
    {
         qDebug() << "Connection failed";
    }
    qDebug() << "Conection ended";
}
