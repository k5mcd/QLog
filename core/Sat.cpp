#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QDebug>
#include <QStringRef>
#include <QDir>
#include <QFile>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include "Sat.h"
#include "debug.h"

#define SAT_URL "https://raw.githubusercontent.com/foldynl/QLog-data/main/sats/satslist.csv"
#define SAT_FILE_AGING 30

MODULE_IDENTIFICATION("qlog.core.sat");

Sat::Sat() {
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(processReply(QNetworkReply*)));
}

void Sat::update() {
    FCT_IDENTIFICATION;

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    QSettings settings;
    QDate last_update = settings.value("last_sat_update").toDate();

    if ( dir.exists("satslist.csv")
         && last_update.isValid()
         && last_update.daysTo(QDate::currentDate()) < SAT_FILE_AGING )
    {
        if ( isSatFilled() )
        {
            // nothing to do.
            qCDebug(runtime) << "Not needed to update";
            emit noUpdate();
            return;
        }
        qCDebug(runtime) << "use cached satslist.csv at" << dir.path();
        QTimer::singleShot(0, this, &Sat::loadData);
    }
    else
    {
        qCDebug(runtime) << "Satlist is too old or not exist ";
        download();
    }
}

void Sat::loadData() {
    FCT_IDENTIFICATION;

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile file(dir.filePath("satslist.csv"));
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    parseData(stream);
    file.close();

    emit finished(true);
}

void Sat::download() {
    FCT_IDENTIFICATION;

    QUrl url(SAT_URL);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "QLog/1.0 (Qt)");
    nam->get(request);
    qCDebug(runtime) << "download satslist.csv from" << url.toString();
}

void Sat::processReply(QNetworkReply* reply) {
    FCT_IDENTIFICATION;

    QByteArray data = reply->readAll();

    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        qCDebug(runtime) << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qCDebug(runtime) << reply->header(QNetworkRequest::KnownHeaders::LocationHeader);

        QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

        QFile file(dir.filePath("satslist.csv"));
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.flush();
        file.close();
        reply->deleteLater();

        QSettings settings;
        settings.setValue("last_sat_update", QDateTime::currentDateTimeUtc().date());

        loadData();
    }
    else {
        qCDebug(runtime) << "failed to download satslist.csv";

        reply->deleteLater();
        emit finished(false);
    }

}

bool Sat::isSatFilled()
{
    FCT_IDENTIFICATION;
    QSqlQuery query(QString("select exists( select 1 from sat_info)"));
    int i = query.first() ? query.value(0).toInt() : 0;
    qCDebug(runtime) << i;
    return i==1;
}

void Sat::deleteSatTable()
{
    FCT_IDENTIFICATION;
    QSqlQuery query;

    query.exec("delete from sat_info");
}

void Sat::parseData(QTextStream& data) {
    FCT_IDENTIFICATION;

    QSqlDatabase::database().transaction();

    deleteSatTable();

    QSqlTableModel entityTableModel;
    entityTableModel.setTable("sat_info");
    QSqlRecord entityRecord = entityTableModel.record();

    int count = 0;

    while (!data.atEnd()) {
        QString line = data.readLine();
        QStringList fields = line.split(';');

        if (fields.count() != 8)  {
            qCDebug(runtime) << "Invalid line in satslist.csv";
            continue;
        }

        qCDebug(runtime) << fields;

        entityRecord.clearValues();
        entityRecord.setValue("name", fields.at(0));
        entityRecord.setValue("number", fields.at(1));
        entityRecord.setValue("uplink", fields.at(2));
        entityRecord.setValue("downlink", fields.at(3));
        entityRecord.setValue("beacon", fields.at(4));
        entityRecord.setValue("mode", fields.at(5));
        entityRecord.setValue("callsign", fields.at(6));
        entityRecord.setValue("status", fields.at(7));

        entityTableModel.insertRecord(-1, entityRecord);
        emit progress(count);
        QCoreApplication::processEvents();

        count++;
    }

    if ( entityTableModel.submitAll() )
    {
        QSqlDatabase::database().commit();
        qCDebug(runtime) << "Satlist update finished:" << count << "entities loaded.";
    }
    else
    {
        qCWarning(runtime) << "Satlist update failed - rollback";
        QSqlDatabase::database().rollback();
    }
}

Sat::~Sat() {
    delete nam;
}

int Sat::MAX_ENTITIES = 1100;
