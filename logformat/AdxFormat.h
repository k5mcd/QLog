#ifndef ADXFORMAT_H
#define ADXFORMAT_H

#include "LogFormat.h"

class QXmlStreamWriter;

class AdxFormat : public LogFormat {
public:
    explicit AdxFormat(QTextStream& stream) : LogFormat(stream) {writer = nullptr;}

    void exportContact(const QSqlRecord& record,QMap<QString, QString> *) override;
    void exportStart() override;
    void exportEnd() override;

private:
    void writeField(QString name, QString value);

    QXmlStreamWriter* writer;
};

#endif // ADIF3FORMAT_H
