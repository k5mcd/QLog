#ifndef ADXFORMAT_H
#define ADXFORMAT_H

#include <QXmlStreamWriter>

#include "AdiFormat.h"

class AdxFormat : public AdiFormat
{
public:
    explicit AdxFormat(QTextStream& stream);

    virtual void importStart() override;
    virtual void importEnd() override;

    virtual void exportContact(const QSqlRecord& record,
                               QMap<QString, QString> *applTags = nullptr) override;
    virtual void exportStart() override;
    virtual void exportEnd() override;

protected:
    virtual void writeField(const QString &name,
                            const QString &value,
                            const QString &type="") override;
    virtual void writeSQLRecord(const QSqlRecord& record,
                                QMap<QString, QString> *applTags) override;
    virtual bool readContact(QVariantMap& ) override;

private:
    QXmlStreamWriter *writer;
    QXmlStreamReader *reader;
};

#endif // ADXFORMAT_H
