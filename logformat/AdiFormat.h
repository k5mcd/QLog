#ifndef ADIFORMAT_H
#define ADIFORMAT_H

#include "LogFormat.h"

class AdiFormat : public LogFormat {
public:
    explicit AdiFormat(QTextStream& stream) : LogFormat(stream) {}

    bool readContact(QVariantMap& );

    bool importNext(QSqlRecord& ) override;
    void exportContact(const QSqlRecord&, QMap<QString, QString> * applTags = nullptr) override;
    void exportStart() override;

    static QDate parseDate(const QString &date);
    static QTime parseTime(const QString &time);
    static QString parseQslRcvd(const QString &value);
    static QString parseQslSent(const QString &value);
    static QString parseUploadStatus(const QString &value);

    static void importIntlField(const QString &sourceField,
                                const QString &sourceFieldIntl,
                                QSqlRecord &,
                                QMap<QString, QVariant> &);
    static void fillIntlFields(QSqlRecord &,
                               QMap<QString, QVariant> &);

private:
    enum ParserState {
        START,
        FIELD,
        KEY,
        SIZE,
        DATA_TYPE,
        VALUE
    };

    void writeField(const QString &name, const QString &value, const QString &type="");
    void readField(QString& field,QString& value);

    ParserState state = START;
    bool inHeader = false;
};

#endif // ADIF2FORMAT_H
