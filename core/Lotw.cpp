#include <QUrl>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "Lotw.h"
#include "logformat/AdiFormat.h"
#include "utils.h"
#include "debug.h"

#define ADIF_API "https://lotw.arrl.org/lotwuser/lotwreport.adi"

MODULE_IDENTIFICATION("qlog.core.lotw");

Lotw::Lotw(QObject *parent) : QObject(parent)
{
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished,
            this, &Lotw::processReply);
}

void Lotw::update(QDate start_date, bool qso_since, QString station_callsign)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << start_date << " " << qso_since;

    QList<QPair<QString, QString>> params;
    params.append(qMakePair(QString("qso_query"), QString("1")));
    params.append(qMakePair(QString("qso_qsldetail"), QString("yes")));
    params.append(qMakePair(QString("qso_owncall"), station_callsign));

    QString start = start_date.toString("yyyy-MM-dd");
    if (qso_since) {
        params.append(qMakePair(QString("qso_qsl"), QString("no")));
        params.append(qMakePair(QString("qso_qsorxsince"), start));
    }
    else {
        params.append(qMakePair(QString("qso_qsl"), QString("yes")));
        params.append(qMakePair(QString("qso_qslsince"), start));
    }

    get(params);
}

void Lotw::get(QList<QPair<QString, QString>> params) {
    FCT_IDENTIFICATION;

    QSettings settings;
    QString username = settings.value(Lotw::CONFIG_USERNAME_KEY).toString();
    QString password = getPassword(Lotw::SECURE_STORAGE_KEY, username);

    QUrlQuery query;
    query.setQueryItems(params);
    query.addQueryItem("login", username);
    query.addQueryItem("password", password);

    QUrl url(ADIF_API);
    url.setQuery(query);

    qCDebug(runtime) << url.toString();

    nam->get(QNetworkRequest(url));
}

void Lotw::processReply(QNetworkReply* reply) {
    FCT_IDENTIFICATION;

    if (reply->error() != QNetworkReply::NoError)
    {
        qCDebug(runtime) << "LotW error" << reply->errorString();
        reply->deleteLater();
        emit updateFailed();
        return;
    }

    qint64 size = reply->size();
    qCDebug(runtime) << "Reply received, size: " << size;

    /* Currently, QT returns an incorrect stream position value in Network stream
     * when the stream is used in QTextStream. Therefore
     * QLog downloads a response, saves it to a temp file and opens
     * the file as a stream */
    QTemporaryFile tempFile;

    if ( ! tempFile.open() )
    {
        qCDebug(runtime) << "Cannot open temp file";
        emit updateFailed();
        return;
    }

    QByteArray data = reply->readAll();

    tempFile.write(data);
    tempFile.flush();
    tempFile.seek(0);

    emit updateStarted();

    /* see above why QLog uses a temp file */
    QTextStream stream(&tempFile);
    AdiFormat adi(stream);

    connect(&adi, &AdiFormat::progress, [this, size](qint64 position)
    {
        if ( size > 0 )
        {
            double progress = position * 100.0 / size;
            emit updateProgress(static_cast<int>(progress));
        }
    });

    connect(&adi, &AdiFormat::QSLMergeFinished, [this](QSLMergeStat stats)
    {
        emit updateComplete(stats);
    });

    adi.runQSLImport(adi.LOTW);

    tempFile.close();

    reply->deleteLater();
}

const QString Lotw::SECURE_STORAGE_KEY = "QLog: LoTW";
const QString Lotw::CONFIG_USERNAME_KEY = "lotw/username";
