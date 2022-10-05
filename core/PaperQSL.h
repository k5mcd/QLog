#ifndef PAPERQSL_H
#define PAPERQSL_H

#include <QObject>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QFileInfoList>

class PaperQSL : public QObject
{
    Q_OBJECT
public:
    explicit PaperQSL(QObject *parent = nullptr);

    QFileInfoList getQSLFileList(const QSqlRecord);
    static QString getQSLImageFolder(const QString &defaultPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
    static void saveQSLImageFolder(const QString &path);
    QString stripBaseFileName(const QString &);
    bool addQSLFile(const QString &, const QSqlRecord, QString &);

signals:

private:

    QString getQSLFilenameFilter(const QSqlRecord);
    static const QString CONFIG_QSL_FOLDER_KEY;
};

#endif // PAPERQSL_H
