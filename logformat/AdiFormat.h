#ifndef ADIFORMAT_H
#define ADIFORMAT_H

#include "LogFormat.h"

class AdiFormat : public LogFormat {
public:
    explicit AdiFormat(QTextStream& stream) : LogFormat(stream) {}

    bool readContact(QVariantMap& contact);

    bool importNext(QSqlRecord& contact) override;
    void exportContact(const QSqlRecord&record, QMap<QString, QString> * applTags = nullptr) override;
    void exportStart() override;

    static QDate parseDate(const QString &date);
    static QTime parseTime(const QString &time);
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
    QString parseQslRcvd(const QString &value);
    QString parseQslSent(const QString &value);

    ParserState state = START;
    bool inHeader = false;
};

#endif // ADIF2FORMAT_H
