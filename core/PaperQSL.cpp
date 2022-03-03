#include <QDir>
#include <QSettings>
#include <QDateTime>
#include <QRegularExpression>

#include "PaperQSL.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.core.paperqsl");

PaperQSL::PaperQSL(QObject *parent) : QObject(parent)
{
    FCT_IDENTIFICATION;
}

QFileInfoList PaperQSL::getQSLFileList(const QSqlRecord qso)
{
    FCT_IDENTIFICATION;

    QDir dir(getQSLImageFolder());
    QFileInfoList file_list = dir.entryInfoList(QStringList(getQSLFilenameFilter(qso)), QDir::Files, QDir::Name);
    return file_list;
}

QString PaperQSL::getQSLImageFolder(const QString &defaultPath)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    return settings.value(PaperQSL::CONFIG_QSL_FOLDER_KEY, defaultPath).toString();
}

void PaperQSL::saveQSLImageFolder(const QString &path)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    settings.setValue(PaperQSL::CONFIG_QSL_FOLDER_KEY, path);

}

QString PaperQSL::stripBaseFileName(const QString &filename)
{
    FCT_IDENTIFICATION;

    QRegularExpression re("^[0-9]{8}_[0-9]+_.*_qsl_(.*)", QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch match = re.match(filename);

    if ( match.hasMatch() )
    {
        return match.captured(1);
    }

    qCInfo(runtime) << "Filename has unexpected filename format: " << filename;

    return QString();
}

bool PaperQSL::addQSLFile(const QString &filename, const QSqlRecord qso, QString &newFilename)
{
    FCT_IDENTIFICATION;

    QFileInfo srcFile(filename);

    if ( ! srcFile.exists() )
    {
        qCDebug(runtime) << filename << " does not exist";
        return false;
    }

    QDateTime time_start = qso.value("start_time").toDateTime().toTimeSpec(Qt::UTC);

    newFilename = getQSLImageFolder() + QDir::separator() + QString("%1_%2_%3_qsl_%4").arg(time_start.toString("yyyyMMdd"),
                                                                                           qso.value("id").toString(),
                                                                                           qso.value("callsign").toString(),
                                                                                           srcFile.fileName());
    qCDebug(runtime) << "new filename " << newFilename;

    return QFile::copy(filename, newFilename);
}

QString PaperQSL::getQSLFilenameFilter(const QSqlRecord qso)
{
    FCT_IDENTIFICATION;

    QDateTime time_start = qso.value("start_time").toDateTime().toTimeSpec(Qt::UTC);

    QString ret = QString("%1_%2_%3_qsl_*.*").arg(time_start.toString("yyyyMMdd"),
                                                   qso.value("id").toString(),
                                                   qso.value("callsign").toString());

    qCDebug(runtime) << "PaperQSL Filename filter: " << ret;

    return ret;
}


const QString PaperQSL::CONFIG_QSL_FOLDER_KEY = "paperqsl/qslfolder";
