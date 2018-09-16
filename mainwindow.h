#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "loadingthread.h"
#include <QLineSeries>
#include <QDialog>
#include <QtCharts>
#include <QUrl>



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
    void on_pushButton_clicked();

    void on_ySlider_sliderMoved(int position);

    void on_yEdit_editingFinished();

    void on_xSlider_sliderMoved(int position);

    void on_xEdit_editingFinished();

    void setChart();

    void requestFinished(QNetworkReply*);
    void requestError(QNetworkReply::NetworkError err);
    void onDownloadProgress(qint64,qint64);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    loadingThread *lThread;
    QNetworkReply *reply;

public slots:
    void onDataChanged(QLineSeries*);

};

#endif // MAINWINDOW_H
