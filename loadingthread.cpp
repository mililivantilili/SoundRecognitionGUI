
#include "loadingthread.h"
#include <QtCore>
#include <QNetworkAccessManager>
#include <QtNetwork>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QLineSeries>
#include <QtCharts>
#include <QObject>
#include <QString>
#include <QStringList>


#define SAMPLE_NUM 5000

QUrl url;
QString localReply;
int networkIsRunningFor = 0;

loadingThread::loadingThread(QObject *parent)
{

}

void loadingThread::run()
{
    QLineSeries *series = new QLineSeries();
    qDebug() << "run";
    QStringList replyList;

    while(work)
    {
        if(calculate)
        {
            calculate = false;
            QRegExp rx("[ ]");
            replyList = localReply.split(rx, QString::SkipEmptyParts);
            series->clear();
            series->blockSignals(true);
            for(int i = 0; i < replyList.length(); i++){
                *series << QPointF(i, replyList[i].toUShort());
            }
            series->blockSignals(false);
            emit dataChanged(series);
            qDebug() << "11";
        }

        msleep(100);

    }
}

void loadingThread::calculateGraph(QString reply)
{
    localReply = reply;
}
