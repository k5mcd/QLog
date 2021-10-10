#ifndef CTY_H
#define CTY_H

#include <QtCore>
#include <QObject>
#include "data/Dxcc.h"

class QNetworkAccessManager;
class QNetworkReply;

class Cty : public QObject {
    Q_OBJECT

public:
    Cty();
    ~Cty();

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
    bool isDXCCFilled();
    void deleteDXCCTables();

    QNetworkAccessManager* nam;
};

#endif // CTY_H
