#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include "Cty.h"
#include "debug.h"

#define CTY_URL "http://www.country-files.com/cty/cty.csv"
#define CTY_FILE_AGING 21

MODULE_IDENTIFICATION("qlog.core.cty");

Cty::Cty(QObject *parent) :
    QObject(parent)
{
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished, this, &Cty::processReply);
}

void Cty::update() {
    FCT_IDENTIFICATION;

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));

    QSettings settings;
    QDate last_update = settings.value("last_cty_update").toDate();

    if ( dir.exists("cty.csv")
         && last_update.isValid()
         && last_update.daysTo(QDate::currentDate()) < CTY_FILE_AGING )
    {
        if ( isDXCCFilled() )
        {
            // nothing to do.
            qCDebug(runtime) << "Not needed to update";
            emit noUpdate();
            return;
        }
        qCDebug(runtime) << "use cached cty.csv at" << dir.path();
        QTimer::singleShot(0, this, &Cty::loadData);
    }
    else
    {
        qCDebug(runtime) << "CTY is too old or not exist ";
        download();
    }
}

void Cty::loadData() {
    FCT_IDENTIFICATION;

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    QFile file(dir.filePath("cty.csv"));
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    parseData(stream);
    file.close();

    emit finished(true);
}

void Cty::download() {
    FCT_IDENTIFICATION;

    QUrl url(CTY_URL);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "QLog/1.0 (Qt)");
    nam->get(request);
    qCDebug(runtime) << "download cty.csv from" << url.toString();
}

void Cty::processReply(QNetworkReply* reply) {
    FCT_IDENTIFICATION;

    QByteArray data = reply->readAll();

    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        qCDebug(runtime) << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qCDebug(runtime) << reply->header(QNetworkRequest::KnownHeaders::LocationHeader);

        QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));

        QFile file(dir.filePath("cty.csv"));
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.flush();
        file.close();
        reply->deleteLater();

        QSettings settings;
        settings.setValue("last_cty_update", QDateTime::currentDateTimeUtc().date());

        loadData();
    }
    else {
        qCDebug(runtime) << "failed to download cty.csv";

        reply->deleteLater();
        emit finished(false);
    }

}

bool Cty::isDXCCFilled()
{
    FCT_IDENTIFICATION;
    QSqlQuery query(QString("select exists( select 1 from dxcc_entities )"));
    int i = query.first() ? query.value(0).toInt() : 0;
    qCDebug(runtime) << i;
    return i==1;
}

void Cty::deleteDXCCTables()
{
    FCT_IDENTIFICATION;
    QSqlQuery query;

    if ( ! query.exec("delete from dxcc_prefixes") )
    {
        qWarning() << "Cannot delete dxcc_prefixes " << query.lastError();

    }

    query.clear();

    if ( ! query.exec("delete from dxcc_entities") )
    {
        qWarning() << "Cannot delete dxcc_entities " << query.lastError();
    }
}

void Cty::parseData(QTextStream& data) {
    FCT_IDENTIFICATION;

    static QRegularExpression prefixSeperator("[\\s;]");
    static QRegularExpression prefixFormat("(=?)([A-Z0-9/]+)(?:\\((\\d+)\\))?(?:\\[(\\d+)\\])?$");
    QRegularExpressionMatch matchExp;

    QSqlDatabase::database().transaction();

    deleteDXCCTables();

    QSqlTableModel entityTableModel;
    entityTableModel.setTable("dxcc_entities");
    QSqlRecord entityRecord = entityTableModel.record();

    QSqlTableModel prefixTableModel;
    prefixTableModel.setTable("dxcc_prefixes");
    prefixTableModel.removeColumn(prefixTableModel.fieldIndex("id"));
    QSqlRecord prefixRecord = prefixTableModel.record();

    int count = 0;

    while (!data.atEnd()) {
        QString line = data.readLine();
        QStringList fields = line.split(',');

        if (fields.count() != 10)  {
            qCDebug(runtime) << "Invalid line in cty.csv";
            continue;
        }
        else if (fields.at(0).startsWith("*")) {
            continue;
        }

        qCDebug(runtime) << fields;

        int dxcc_id = fields.at(2).toInt();

        entityRecord.clearValues();
        entityRecord.setValue("id", dxcc_id);
        entityRecord.setValue("prefix", fields.at(0));
        entityRecord.setValue("name", fields.at(1));
        entityRecord.setValue("cont", fields.at(3));
        entityRecord.setValue("cqz", fields.at(4));
        entityRecord.setValue("ituz", fields.at(5));
        entityRecord.setValue("lat", fields.at(6).toFloat());
        entityRecord.setValue("lon", -fields.at(7).toFloat());
        entityRecord.setValue("tz", fields.at(8).toFloat());
        entityTableModel.insertRecord(-1, entityRecord);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
        QStringList prefixList = fields.at(9).split(prefixSeperator, Qt::SkipEmptyParts);
#else /* Due to ubuntu 20.04 where qt5.12 is present */
        QStringList prefixList = fields.at(9).split(prefixSeperator, QString::SkipEmptyParts);
#endif
        qCDebug(runtime) << prefixList;

        for (auto &prefix : qAsConst(prefixList))
        {
            matchExp = prefixFormat.match(prefix);
            if ( matchExp.hasMatch() )
            {
                prefixRecord.clearValues();
                prefixRecord.setValue("dxcc", dxcc_id);
                prefixRecord.setValue("exact", !matchExp.captured(1).isEmpty());
                prefixRecord.setValue("prefix", matchExp.captured(2));
                prefixRecord.setValue("cqz", matchExp.captured(3).toInt());
                prefixRecord.setValue("ituz", matchExp.captured(4).toInt());

                prefixTableModel.insertRecord(-1, prefixRecord);
            }
            else
            {
                qCDebug(runtime) << "Failed to match " << prefix;
            }
        }

        emit progress(count);
        QCoreApplication::processEvents();

        count++;
    }

    if ( entityTableModel.submitAll()
         && entityTableModel.submitAll() )
    {
        qCDebug(runtime) << "DXCC update finished:" << count << "entities loaded.";
        QSqlDatabase::database().commit();
    }
    else
    {
        qCWarning(runtime) << "DXCC update failed - rollback";
        QSqlDatabase::database().rollback();
    }
}

Cty::~Cty() {
    delete nam;
}

int Cty::MAX_ENTITIES = 350;
