#ifndef SAT_H
#define SAT_H

#include <QtCore>
#include <QObject>
#include "data/Dxcc.h"

class QNetworkAccessManager;
class QNetworkReply;

class Sat : public QObject {
    Q_OBJECT

public:
    Sat();
    ~Sat();

    static int MAX_ENTITIES;

public slots:
    void loadData();
    void processReply(QNetworkReply* reply);
    void update();

signals:
    void progress(int count);
    void finished(bool result);
    void noUpdate();

private:
    void download();
    void parseData(QTextStream& data);
    bool isSatFilled();
    void deleteSatTable();

    QNetworkAccessManager* nam;
};

#endif // SAT_H
