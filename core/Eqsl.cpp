#include <QUrl>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QHttpMultiPart>
#include "Eqsl.h"
#include "core/debug.h"
#include "core/CredentialStore.h"
#include "logformat/AdiFormat.h"

#define DOWNLOAD_1ST_PAGE "https://www.eQSL.cc/qslcard/DownloadInBox.cfm"
#define DOWNLOAD_2ND_PAGE "https://www.eQSL.cc/downloadedfiles/"
#define UPLOAD_ADIF_PAGE "https://www.eQSL.cc/qslcard/ImportADIF.cfm"

MODULE_IDENTIFICATION("qlog.core.eqsl");

EQSL::EQSL( QObject *parent )
{
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished,
            this, &EQSL::processReply);
}

void EQSL::update(QDate start_date, QString qthNick)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << start_date << " " << qthNick;

    QList<QPair<QString, QString>> params;
    params.append(qMakePair(QString("QTHNickname"), qthNick));

    QString start = start_date.toString("yyyyMMdd");
    if (start_date.isValid())
    {
        params.append(qMakePair(QString("RcvdSince"), start));
    }

    get(params);
}

int EQSL::uploadAdif(QByteArray &data)
{
    FCT_IDENTIFICATION;

    QSettings settings;
    QString username = settings.value(EQSL::CONFIG_USERNAME_KEY).toString();
    QString password = CredentialStore::instance()->getPassword(EQSL::SECURE_STORAGE_KEY,
                                                                username);

    /* http://www.eqsl.cc/qslcard/ImportADIF.txt */
    //qInfo() << qPrintable(data);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

    /* UserName */
    QHttpPart userPart;
    userPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"eqsl_user\""));
    userPart.setBody(username.toUtf8());

    /* Password */
    QHttpPart passPart;
    passPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"eqsl_pswd\""));
    passPart.setBody(password.toUtf8());

    /* File */
    QTemporaryFile file;
    file.open();
    file.write(data);
    file.flush();

    QHttpPart filePart;
    QString aux = QString("form-data; name=\"Filename\"; filename=\"%1\"").arg(file.fileName());
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(aux));
    filePart.setBody(data);

    multiPart->append(userPart);
    multiPart->append(passPart);
    multiPart->append(filePart);

    QNetworkRequest request(QUrl(UPLOAD_ADIF_PAGE));

    QNetworkReply* reply = nam->post(request, multiPart);
    reply->setProperty("messageType", QVariant("uploadADIFFile"));

    return 0;
}

void EQSL::get(QList<QPair<QString, QString>> params)
{
    FCT_IDENTIFICATION;

    QSettings settings;
    QString username = settings.value(EQSL::CONFIG_USERNAME_KEY).toString();
    QString password = CredentialStore::instance()->getPassword(EQSL::SECURE_STORAGE_KEY,
                                                                username);

    QUrlQuery query;
    query.setQueryItems(params);
    query.addQueryItem("UserName", username);
    query.addQueryItem("Password", password);

    QUrl url(DOWNLOAD_1ST_PAGE);
    url.setQuery(query);

    qCDebug(runtime) << url.toString();

    QNetworkReply* reply = nam->get(QNetworkRequest(url));
    reply->setProperty("messageType", QVariant("getADIFFileName"));
}

void EQSL::downloadADIF(QString filename)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << filename;

    QUrlQuery query;
    QUrl url(DOWNLOAD_2ND_PAGE + filename);
    url.setQuery(query);

    qCDebug(runtime) << url.toString();

    QNetworkReply* reply = nam->get(QNetworkRequest(url));
    reply->setProperty("messageType", QVariant("getADIF"));
}

void EQSL::processReply(QNetworkReply* reply)
{
    FCT_IDENTIFICATION;

    if ( reply->error() != QNetworkReply::NoError)
    {
        qCDebug(runtime) << "eQSL error URL " << reply->request().url().toString();
        qCDebug(runtime) << "eQSL error" << reply->errorString();
        reply->deleteLater();
        emit updateFailed(reply->errorString());
        return;
    }

    QString messageType = reply->property("messageType").toString();

    qCDebug(runtime) << "Received Message Type: " << messageType;

    /*******************/
    /* getADIFFileName */
    /*******************/
    if ( messageType == "getADIFFileName" )
    {
        //getting the first page where a ADIF filename is present

        QString replayString(reply->readAll());

        if ( replayString.contains("No such Username/Password found") )
        {
            qCDebug(runtime) << "Incorrect Password or QTHProfile Id";
            emit updateFailed(tr("Incorrect Password or QTHProfile Id"));
        }
        else
        {
            QRegularExpression re("/downloadedfiles/(.*.adi)\">.ADI file");
            QRegularExpressionMatch match = re.match(replayString);

            if ( match.hasMatch() )
            {
                QString filename = match.captured(1);
                downloadADIF(filename);
            }
            else
            {
                qCInfo(runtime) << "File not found in HTTP reply ";
                emit updateFailed(tr("ADIF file not found in eQSL response"));
            }
        }
    }
    /***********/
    /* getADIF */
    /***********/
    else if ( messageType == "getADIF")
    {
        qint64 size = reply->size();
        qCDebug(runtime) << "Reply size: " << size;

        /* Currently, QT returns an incorrect stream position value in Network stream
         * when the stream is used in QTextStream. Therefore
         * QLog downloads a response, saves it to a temp file and opens
         * the file as a stream */
        QTemporaryFile tempFile;

        if ( ! tempFile.open() )
        {
            qCDebug(runtime) << "Cannot open temp file";
            emit updateFailed(tr("Cannot opet temporary file"));
            reply->deleteLater();
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

        adi.runQSLImport(adi.EQSL);

        tempFile.close();
    }
    /******************/
    /* uploadADIFFile */
    /******************/
    else if ( messageType == "uploadADIFFile" )
    {
        QString replayString(reply->readAll());
        QRegularExpression rOK("Result: (.*)");
        QRegularExpressionMatch matchOK = rOK.match(replayString);
        QRegularExpression rError("Error: (.*)");
        QRegularExpressionMatch matchError = rError.match(replayString);
        QRegularExpression rWarning("Warning: (.*)");
        QRegularExpressionMatch matchWarning = rWarning.match(replayString);
        QRegularExpression rCaution("Caution: (.*)");
        QRegularExpressionMatch matchCaution = rCaution.match(replayString);
        QString msg;

        if ( matchOK.hasMatch() )
        {
            msg = matchOK.captured(1);
            uploadOK(msg);
        }
        else if (matchError.hasMatch() )
        {
            msg = matchError.captured(1);
            uploadError(msg);
        }
        else if (matchWarning.hasMatch() )
        {
            msg = matchWarning.captured(1);
            uploadOK(msg);
        }
        else if (matchCaution.hasMatch() )
        {
            msg = matchCaution.captured(1);
            uploadError(msg);
        }
        else
        {
            qCInfo(runtime) << "Unknown Reply ";
            qCInfo(runtime) << replayString;
            emit uploadError(tr("Unknown Reply from eQSL"));
        }
    }

    reply->deleteLater();
}

const QString EQSL::SECURE_STORAGE_KEY = "QLog:eQSL";
const QString EQSL::CONFIG_USERNAME_KEY = "eqsl/username";
