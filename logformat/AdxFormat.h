#ifndef ADXFORMAT_H
#define ADXFORMAT_H

#include "LogFormat.h"

class QXmlStreamWriter;

class AdxFormat : public LogFormat {
public:
    explicit AdxFormat(QTextStream& stream) : LogFormat(stream) {writer = nullptr; reader = nullptr;}

    virtual void importStart() override;
    virtual void importEnd() override;
    virtual bool importNext(QSqlRecord &record) override;

    void exportContact(const QSqlRecord& record,QMap<QString, QString> *) override;
    void exportStart() override;
    void exportEnd() override;

private:
    void writeField(QString name, QString value);
    bool readContact(QVariantMap& );

    QXmlStreamWriter* writer;
    QXmlStreamReader* reader;
};

#endif // ADIF3FORMAT_H
