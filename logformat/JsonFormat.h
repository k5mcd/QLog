#ifndef JSONFORMAT_H
#define JSONFORMAT_H

#include <QJsonArray>
#include "AdxFormat.h"

class JsonFormat : public AdxFormat
{
public:
    explicit JsonFormat(QTextStream& stream) : AdxFormat(stream) {}

    virtual bool importNext(QSqlRecord& contact) override;
    virtual void importStart() override {};
    virtual void importEnd() override {};

    virtual void exportStart() override;
    virtual void exportContact(const QSqlRecord& record, QMap<QString, QString> *) override;
    virtual void exportEnd() override;

protected:
    virtual void writeField(const QString &name,
                            bool presenceCondition,
                            const QString &value,
                            const QString &type="") override;

private:
   QJsonArray data;
   QJsonObject contact;
};

#endif // JSONFORMAT_H
