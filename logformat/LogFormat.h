#ifndef LOGFORMAT_H
#define LOGFORMAT_H

#include <QtCore>
#include <QMap>

class QSqlRecord;

struct QSLMergeStat {
    QStringList newQSLs;
    QStringList unmatchedQSLs;
    int qsos_updated;
    int qsos_checked;
    int qsos_unmatched;
    int qsos_errors;
};

class LogFormat : public QObject {
    Q_OBJECT

public:
    enum Type {
        ADI,
        ADX,
        CABRILLO,
        JSON
    };

    enum QSLFrom {
        LOTW,
        EQSL,
        UNKNOW
    };

    enum duplicateQSOBehaviour {
        ASK_NEXT,
        SKIP_ONE,
        SKIP_ALL,
        ACCEPT_ONE,
        ACCEPT_ALL
    };

    LogFormat(QTextStream& stream);

    virtual ~LogFormat();

    static LogFormat* open(QString type, QTextStream& stream);
    static LogFormat* open(Type type, QTextStream& stream);

    int runImport();
    void runQSLImport(QSLFrom fromService);
    int runExport();
    int runExport(const QList<QSqlRecord>&);
    void setDefaults(QMap<QString, QString>& defaults);
    void setDateRange(QDate start, QDate end);
    void setUpdateDxcc(bool updateDxcc);
    void setDuplicateQSOCallback(duplicateQSOBehaviour (*func)(QSqlRecord *, QSqlRecord *));

    virtual void importStart() {}
    virtual void importEnd() {}
    virtual bool importNext(QSqlRecord&) { return false; }

    virtual void exportStart() {}
    virtual void exportEnd() {}
    virtual void exportContact(const QSqlRecord&, QMap<QString, QString> * = nullptr) {}

signals:
    void importPosition(qint64 value);
    void exportProgress(float value);
    void finished(int count);
    void QSLMergeFinished(QSLMergeStat stats);

protected:
    QTextStream& stream;
    QMap<QString, QString>* defaults;

private:
    bool dateRangeSet();
    bool inDateRange(QDate date);
    QDate startDate, endDate;
    bool updateDxcc = false;
    duplicateQSOBehaviour (*duplicateQSOFunc)(QSqlRecord *, QSqlRecord *);
};

#endif // LOGFORMAT_H
