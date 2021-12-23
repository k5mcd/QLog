#ifndef JSONFORMAT_H
#define JSONFORMAT_H

#include "LogFormat.h"

class QJsonArray;

class JsonFormat : public LogFormat {
public:
    explicit JsonFormat(QTextStream& stream) : LogFormat(stream) {}

    bool importNext(QSqlRecord& contact) override;
    void exportContact(const QSqlRecord& record, QMap<QString, QString> *) override;
    void exportEnd() override;

private:
   QJsonArray data;
};

#endif // JSONFORMAT_H
