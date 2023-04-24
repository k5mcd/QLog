#ifndef ADIFORMAT_H
#define ADIFORMAT_H

#include "LogFormat.h"

class AdiFormat : public LogFormat
{
public:
    explicit AdiFormat(QTextStream& stream) : LogFormat(stream) {}

    virtual bool importNext(QSqlRecord& ) override;

    virtual void exportContact(const QSqlRecord&,
                               QMap<QString, QString> *applTags = nullptr) override;
    virtual void exportStart() override;

protected:
    virtual void writeField(const QString &name,
                            const QString &value,
                            const QString &type="");
    virtual void writeSQLRecord(const QSqlRecord& record,
                                QMap<QString, QString> *applTags);
    virtual bool readContact(QVariantMap &);
    void mapContact2SQLRecord(QMap<QString, QVariant> &contact,
                              QSqlRecord &record);
    void contactFields2SQLRecord(QMap<QString, QVariant> &contact,
                              QSqlRecord &record);
private:

    void readField(QString& field,
                   QString& value);
    QDate parseDate(const QString &date);
    QTime parseTime(const QString &time);
    QString parseQslRcvd(const QString &value);
    QString parseQslSent(const QString &value);
    QString parseUploadStatus(const QString &value);
    void preprocessINTLFields(QMap<QString, QVariant> &contact);
    void preprocessINTLField(const QString &sourceField,
                             const QString &sourceFieldIntl,
                             QMap<QString, QVariant> &);
    enum ParserState {
        START,
        FIELD,
        KEY,
        SIZE,
        DATA_TYPE,
        VALUE
    };

    ParserState state = START;
    bool inHeader = false;
};

#endif // ADIF2FORMAT_H
