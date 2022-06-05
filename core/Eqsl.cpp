#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QHttpMultiPart>
#include <QSqlRecord>
#include <QDir>

#include "Eqsl.h"
#include "core/debug.h"
#include "core/CredentialStore.h"
#include "logformat/AdiFormat.h"

#define DOWNLOAD_1ST_PAGE "https://www.eQSL.cc/qslcard/DownloadInBox.cfm"
#define DOWNLOAD_2ND_PAGE "https://www.eQSL.cc/downloadedfiles/"
#define UPLOAD_ADIF_PAGE "https://www.eQSL.cc/qslcard/ImportADIF.cfm"
#define QSL_IMAGE_FILENAME_PAGE "https://www.eQSL.cc/qslcard/GeteQSL.cfm"
#define QSL_IMAGE_DOWNLOAD_PAGE "https://www.eQSL.cc"

MODULE_IDENTIFICATION("qlog.core.eqsl");

EQSL::EQSL( QObject *parent ):
   QObject(parent),
   currentReply(nullptr)
{
    FCT_IDENTIFICATION;

    nam = new QNetworkAccessManager(this);
    connect(nam, &QNetworkAccessManager::finished,
            this, &EQSL::processReply);
}

EQSL::~EQSL()
{
    FCT_IDENTIFICATION;

    nam->deleteLater();

    if ( currentReply )
    {
        currentReply->abort();
        currentReply->deleteLater();
    }
}

void EQSL::update(QDate start_date, QString qthNick)
{
    FCT_IDENTIFICATION;
    qCDebug(function_parameters) << start_date << " " << qthNick;

    QList<QPair<QString, QString>> params;
    if ( !qthNick.isEmpty() )
    {
        params.append(qMakePair(QString("QTHNickname"), qthNick));
    }

    QString start = start_date.toString("yyyyMMdd");
    if (start_date.isValid())
    {
        params.append(qMakePair(QString("RcvdSince"), start));
    }

    get(params);
}

void EQSL::uploadAdif(QByteArray &data)
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

    if ( currentReply )
    {
        qCWarning(runtime) << "processing a new request but the previous one hasn't been completed yet !!!";
    }

    currentReply = nam->post(request, multiPart);
    currentReply->setProperty("messageType", QVariant("uploadADIFFile"));
}

void EQSL::getQSLImage(QSqlRecord qso)
{
    FCT_IDENTIFICATION;

    QString inCacheFilename;

    if ( isQSLImageInCache(qso, inCacheFilename) )
    {
        emit QSLImageFound(inCacheFilename);
        return;
    }

    /* QSL image is not in Cache */
    qCDebug(runtime) << "QSL is not in Cache";

    QSettings settings;
    QString username = settings.value(EQSL::CONFIG_USERNAME_KEY).toString();
    QString password = CredentialStore::instance()->getPassword(EQSL::SECURE_STORAGE_KEY,
                                                                username);

    QDateTime time_start = qso.value("start_time").toDateTime().toTimeSpec(Qt::UTC);

    QUrlQuery query;

    query.addQueryItem("UserName", username);
    query.addQueryItem("Password", password);
    query.addQueryItem("CallsignFrom", qso.value("callsign").toString());
    query.addQueryItem("QSOYear", time_start.toString("yyyy"));
    query.addQueryItem("QSOMonth", time_start.toString("MM"));
    query.addQueryItem("QSODay", time_start.toString("dd"));
    query.addQueryItem("QSOHour", time_start.toString("hh"));
    query.addQueryItem("QSOMinute", time_start.toString("mm"));
    query.addQueryItem("QSOBand", qso.value("band").toString());
    query.addQueryItem("QSOMode", qso.value("mode").toString());

    QUrl url(QSL_IMAGE_FILENAME_PAGE);
    url.setQuery(query);

    qCDebug(runtime) << url.toString();

    if ( currentReply )
    {
        qCWarning(runtime) << "processing a new request but the previous one hasn't been completed yet !!!";
    }

    currentReply = nam->get(QNetworkRequest(url));
    currentReply->setProperty("messageType", QVariant("getQSLImageFileName"));
    currentReply->setProperty("onDiskFilename", QVariant(inCacheFilename));

}

const QString EQSL::getUsername()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(EQSL::CONFIG_USERNAME_KEY).toString();

}

const QString EQSL::getPassword()
{
    FCT_IDENTIFICATION;

    return CredentialStore::instance()->getPassword(EQSL::SECURE_STORAGE_KEY,
                                                    getUsername());
}

const QString EQSL::getQSLImageFolder(const QString &defaultPath)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(EQSL::CONFIG_QSL_FOLDER_KEY, defaultPath).toString();
}

void EQSL::saveUsernamePassword(const QString &newUsername, const QString &newPassword)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    QString oldUsername = getUsername();

    if ( oldUsername != newUsername )
    {
        CredentialStore::instance()->deletePassword(EQSL::SECURE_STORAGE_KEY,
                                                    oldUsername);
    }
    settings.setValue(EQSL::CONFIG_USERNAME_KEY, newUsername);
    CredentialStore::instance()->savePassword(EQSL::SECURE_STORAGE_KEY,
                                              newUsername,
                                              newPassword);
}

void EQSL::saveQSLImageFolder(const QString &path)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue(EQSL::CONFIG_QSL_FOLDER_KEY, path);
}


void EQSL::get(QList<QPair<QString, QString>> params)
{
    FCT_IDENTIFICATION;

    QSettings settings;
    QString username = getUsername();
    QString password = getPassword();

    QUrlQuery query;
    query.setQueryItems(params);
    query.addQueryItem("UserName", username);
    query.addQueryItem("Password", password);

    QUrl url(DOWNLOAD_1ST_PAGE);
    url.setQuery(query);

    qCDebug(runtime) << url.toString();

    if ( currentReply )
    {
        qCWarning(runtime) << "processing a new request but the previous one hasn't been completed yet !!!";
    }

    currentReply = nam->get(QNetworkRequest(url));
    currentReply->setProperty("messageType", QVariant("getADIFFileName"));
}

void EQSL::downloadADIF(const QString &filename)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << filename;

    QUrlQuery query;
    QUrl url(DOWNLOAD_2ND_PAGE + filename);
    url.setQuery(query);

    qCDebug(runtime) << url.toString();

    if ( currentReply )
    {
        qCWarning(runtime) << "processing a new request but the previous one hasn't been completed yet !!!";
    }

    currentReply = nam->get(QNetworkRequest(url));
    currentReply->setProperty("messageType", QVariant("getADIF"));
}

void EQSL::downloadImage(const QString &URLFilename, const QString &onDiskFilename)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << URLFilename << " " << onDiskFilename;

    QUrlQuery query;
    QUrl url(QSL_IMAGE_DOWNLOAD_PAGE + URLFilename);
    url.setQuery(query);

    qCDebug(runtime) << url.toString();

    currentReply = nam->get(QNetworkRequest(url));
    currentReply->setProperty("messageType", QVariant("downloadQSLImage"));
    currentReply->setProperty("onDiskFilename", QVariant(onDiskFilename));
}

QString EQSL::QSLImageFilename(const QSqlRecord qso)
{
    FCT_IDENTIFICATION;

    /* QSL Fileformat YYYYMMDD_ID_Call_eqsl.jpg */

    QDateTime time_start = qso.value("start_time").toDateTime().toTimeSpec(Qt::UTC);

    QString ret = QString("%1_%2_%3_eqsl.jpg").arg(time_start.toString("yyyyMMdd"),
                                                   qso.value("id").toString(),
                                                   qso.value("callsign").toString().replace(QRegularExpression(QString::fromUtf8("[-`~!@#$%^&*()_—+=|:;<>«»,.?/{}\'\"]")),"_"));
    qCDebug(runtime) << "EQSL Image Filename: " << ret;
    return ret;
}

bool EQSL::isQSLImageInCache(QSqlRecord qso, QString &fullPath)
{
    FCT_IDENTIFICATION;

    QDir dir(getQSLImageFolder());
    QString expectingFilename = QSLImageFilename(qso);
    bool isFileExists = dir.exists(expectingFilename);
    fullPath = dir.absoluteFilePath(expectingFilename);

    qCDebug(runtime) << isFileExists << " " << fullPath;

    return isFileExists;
}

void EQSL::processReply(QNetworkReply* reply)
{
    FCT_IDENTIFICATION;

    /* always process one requests per class */
    currentReply = nullptr;

    if ( reply->error() != QNetworkReply::NoError )
    {
        qCDebug(runtime) << "eQSL error URL " << reply->request().url().toString();
        qCDebug(runtime) << "eQSL error" << reply->errorString();

        if ( reply->error() != QNetworkReply::OperationCanceledError )
        {
            emit updateFailed(reply->errorString());
            emit QSLImageError(reply->errorString());
            emit uploadError(reply->errorString());
            reply->deleteLater();
        }
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

        qCDebug(runtime) << replayString;

        if ( replayString.contains("No such Username/Password found")
             || replayString.contains("No such Callsign found") )
        {
            qCDebug(runtime) << "Incorrect Password or QTHProfile Id";
            emit updateFailed(tr("Incorrect Password or QTHProfile Id"));
        }
        else
        {
            static QRegularExpression re("/downloadedfiles/(.*.adi)\">.ADI file");
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
    /***********************/
    /* getQSLImageFileName */
    /***********************/
    else if ( messageType == "getQSLImageFileName" )
    {
        //getting the first page where an Image filename is present

        QString replayString(reply->readAll());

        qCDebug(runtime) << replayString;

        if ( replayString.contains("No such Username/Password found") )
        {
            emit QSLImageError(tr("Incorrect Username or password"));
        }
        else
        {
            static QRegularExpression re("<img src=\"(.*)\" alt");
            QRegularExpressionMatch match = re.match(replayString);

            if ( match.hasMatch() )
            {
                QString filename = match.captured(1);
                QString onDiskFilename = reply->property("onDiskFilename").toString();

                downloadImage(filename,onDiskFilename);
            }
            else
            {
                static QRegularExpression rError("Error: (.*)");
                QRegularExpressionMatch matchError = rError.match(replayString);

                if (matchError.hasMatch() )
                {
                    QString msg = matchError.captured(1);
                    emit QSLImageError(msg);
                }
                else
                {
                    static QRegularExpression rWarning("Warning: (.*) --");
                    QRegularExpressionMatch matchWarning = rWarning.match(replayString);

                    if ( matchWarning.hasMatch() )
                    {
                        QString msg = matchWarning.captured(1);
                        emit QSLImageError(msg);
                    }
                    else
                    {
                        qCInfo(runtime) << replayString;
                        emit QSLImageError(tr("Unknown Error"));
                    }

                }
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

        connect(&adi, &AdiFormat::progress, this, [this, size](qint64 position)
        {
            if ( size > 0 )
            {
                double progress = position * 100.0 / size;
                emit updateProgress(static_cast<int>(progress));
            }
        });

        connect(&adi, &AdiFormat::QSLMergeFinished, this, [this](QSLMergeStat stats)
        {
            emit updateComplete(stats);
        });

        adi.runQSLImport(adi.EQSL);

        tempFile.close();
    }
    /********************/
    /* downloadQSLImage */
    /********************/
    else if ( messageType == "downloadQSLImage")
    {
        qint64 size = reply->size();
        qCDebug(runtime) << "Reply size: " << size;

        QByteArray data = reply->readAll();

        QString onDiskFilename = reply->property("onDiskFilename").toString();

        QFile file(onDiskFilename);
        if ( !file.open(QIODevice::WriteOnly))
        {
            emit QSLImageError("Cannot Save Image to file " + onDiskFilename);
            return;
        }
        file.write(data);
        file.flush();
        file.close();
        emit QSLImageFound(onDiskFilename);
    }
    /******************/
    /* uploadADIFFile */
    /******************/
    else if ( messageType == "uploadADIFFile" )
    {
        QString replayString(reply->readAll());
        qCDebug(runtime) << replayString;

        static QRegularExpression rOK("Result: (.*)");
        QRegularExpressionMatch matchOK = rOK.match(replayString);
        static QRegularExpression rError("Error: (.*)");
        QRegularExpressionMatch matchError = rError.match(replayString);
        static QRegularExpression rWarning("Warning: (.*)");
        QRegularExpressionMatch matchWarning = rWarning.match(replayString);
        static QRegularExpression rCaution("Caution: (.*)");
        QRegularExpressionMatch matchCaution = rCaution.match(replayString);
        QString msg;

        if ( matchOK.hasMatch() )
        {
            msg = matchOK.captured(1);
            emit uploadOK(msg);
        }
        else if (matchError.hasMatch() )
        {
            msg = matchError.captured(1);
            emit uploadError(msg);
        }
        else if (matchWarning.hasMatch() )
        {
            msg = matchWarning.captured(1);
            emit uploadOK(msg);
        }
        else if (matchCaution.hasMatch() )
        {
            msg = matchCaution.captured(1);
            emit uploadError(msg);
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

void EQSL::abortRequest()
{
    FCT_IDENTIFICATION;

    if ( currentReply )
    {
        currentReply->abort();
        //currentReply->deleteLater(); // pointer is deleted later in processReply
        currentReply = nullptr;
    }
}

const QString EQSL::SECURE_STORAGE_KEY = "QLog:eQSL";
const QString EQSL::CONFIG_USERNAME_KEY = "eqsl/username";
const QString EQSL::CONFIG_QSL_FOLDER_KEY = "eqsl/qslfolder";
