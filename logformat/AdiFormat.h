#ifndef ADIFORMAT_H
#define ADIFORMAT_H

#include "LogFormat.h"
#define ADIF_VERSION_STRING "3.1.4"
#define PROGRAMID_STRING "QLog"

class AdiFormat : public LogFormat
{
public:
    explicit AdiFormat(QTextStream& stream) : LogFormat(stream) {}

    virtual bool importNext(QSqlRecord& ) override;

    virtual void exportContact(const QSqlRecord&,
                               QMap<QString, QString> *applTags = nullptr) override;
    virtual void exportStart() override;

    static QMap<QString, QString> fieldname2INTLNameMapping;
    template<typename T>
    static void preprocessINTLFields(T &contact)
    {
        {
            QStringList fieldMappingList = fieldname2INTLNameMapping.keys();
            for ( const QString& fieldName :  qAsConst(fieldMappingList) )
            {
                preprocessINTLField(fieldName, fieldname2INTLNameMapping.value(fieldName), contact);
            }
        }
    }

protected:
    virtual void writeField(const QString &name,
                            bool presenceCondition,
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
    enum ParserState {
        START,
        FIELD,
        KEY,
        SIZE,
        DATA_TYPE,
        VALUE
    };

    static void preprocessINTLField(const QString &sourceField,
                                    const QString &sourceFieldIntl,
                                    QMap<QString, QVariant> &);
    static void preprocessINTLField(const QString &sourceField,
                                    const QString &sourceFieldIntl,
                                    QSqlRecord &);

    ParserState state = START;
    bool inHeader = false;
};

#endif // ADIF2FORMAT_H
