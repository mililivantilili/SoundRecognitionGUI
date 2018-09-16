#ifndef LOADINGTHREAD_H
#define LOADINGTHREAD_H


#include <QObject>
#include <QThread>
#include <QUrl>
#include <QLineSeries>
#include <QtCharts>
#include <QNetworkReply>
#include <QNetworkAccessManager>


class loadingThread : public QThread
{
    Q_OBJECT
public:
    explicit loadingThread(QObject *parent = 0);
    void run();
    bool work;
    bool calculate = false;
    void calculateGraph(QString reply);

signals:
    void dataChanged(QLineSeries*);

private slots:


public slots:

private:


};

#endif // LOADINGTHREAD_H
