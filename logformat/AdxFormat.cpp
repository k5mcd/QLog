#include <QSqlRecord>
#include <QtXml>
#include "logformat/AdxFormat.h"
#include "logformat/AdiFormat.h"
#include "core/debug.h"
#include "data/Data.h"

MODULE_IDENTIFICATION("qlog.logformat.adxformat");

AdxFormat::AdxFormat(QTextStream &stream) :
    AdiFormat(stream),
    writer(nullptr),
    reader(nullptr)
{
    FCT_IDENTIFICATION;
}

void AdxFormat::importStart()
{
    FCT_IDENTIFICATION;

    reader = new QXmlStreamReader(stream.device());

    while ( reader->readNextStartElement() )
    {
        qCDebug(runtime)<<reader->name();
        if ( reader->name() == QString("ADX") )
        {
            while ( reader->readNextStartElement() )
            {
                qCDebug(runtime)<<reader->name();
                if ( reader->name() == QString("HEADER") )
                {
                    reader->skipCurrentElement();
                }
                else if ( reader->name() == QString("RECORDS") )
                {
                    qCDebug(runtime)<<"records found";
                    /* header is loaded, QLog is currently in Records sections
                       which is loaded by importNext procedure */
                    return;
                }
            }
        }
        else
        {
            reader->skipCurrentElement();
        }
    }
}

void AdxFormat::importEnd()
{
    FCT_IDENTIFICATION;

    if ( reader )
    {
        delete reader;
        reader = nullptr;
    }
}

void AdxFormat::exportStart()
{
    FCT_IDENTIFICATION;

    QString date = QDateTime::currentDateTimeUtc().toString("yyyyMMdd hhmmss");

    writer = new QXmlStreamWriter(stream.device());

    writer->setAutoFormatting(true);

    writer->writeStartDocument();
    writer->writeStartElement("ADX");

    writer->writeStartElement("HEADER");
    writer->writeTextElement("ADIF_VER", "3.1.4");
    writer->writeTextElement("PROGRAMID", "QLog");
    writer->writeTextElement("PROGRAMVERSION", VERSION);
    writer->writeTextElement("CREATED_TIMESTAMP", date);
    writer->writeEndElement();

    writer->writeStartElement("RECORDS");
}

void AdxFormat::exportEnd()
{
    FCT_IDENTIFICATION;

    if ( writer )
    {
        writer->writeEndDocument();
        delete writer;
        writer = nullptr;
    }
}

void AdxFormat::exportContact(const QSqlRecord& record, QMap<QString, QString> *applTags)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<record;

    if ( ! writer )
    {
        qWarning() << "ADX Writer is not ready";
        return;
    }

    writer->writeStartElement("RECORD");

    writeSQLRecord(record, applTags);

    writer->writeEndElement();
}

void AdxFormat::writeField(const QString &name,
                           const QString &value,
                           const QString &)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<name << " " << value;

    if (value.isEmpty()) return;
    writer->writeTextElement(name.toUpper(), value);
}

void AdxFormat::writeSQLRecord(const QSqlRecord &record, QMap<QString, QString> *applTags)
{
    FCT_IDENTIFICATION;

    AdiFormat::writeSQLRecord(record, applTags);

    // Add _INTL fields
    writeField("address_intl", record.value("address").toString());
    writeField("comment_intl", record.value("comment_intl").toString());
    writeField("country_intl", record.value("country_intl").toString());
    writeField("my_antenna_intl", record.value("my_antenna_intl").toString());
    writeField("my_city_intl", record.value("my_city_intl").toString());
    writeField("my_country_intl", record.value("my_country_intl").toString());
    writeField("my_name_intl", record.value("my_name_intl").toString());
    writeField("my_postal_code_intl", record.value("my_postal_code_intl").toString());
    writeField("my_rig_intl", record.value("my_rig_intl").toString());
    writeField("my_sig_intl", record.value("my_sig_intl").toString());
    writeField("my_sig_info_intl", record.value("my_sig_info_intl").toString());
    writeField("my_street_intl", record.value("my_street_intl").toString());
    writeField("name_intl", record.value("name_intl").toString());
    writeField("notes_intl", record.value("notes_intl").toString());
    writeField("qslmsg_intl", record.value("qslmsg_intl").toString());
    writeField("qth_intl", record.value("qth_intl").toString());
    writeField("rig_intl", record.value("rig_intl").toString());
    writeField("sig_intl", record.value("sig_intl").toString());
    writeField("sig_info_intl", record.value("sig_info_intl").toString());
}

bool AdxFormat::readContact(QVariantMap & contact)
{
    FCT_IDENTIFICATION;

    while ( !reader->atEnd() )
    {
        reader->readNextStartElement();

        qCDebug(runtime)<<reader->name();

        if ( reader->name() == QString("RECORDS") && reader->isEndElement() )
        {
            qCDebug(runtime)<<"End Records Element";
            return false;
        }
        if ( reader->name() == QString("RECORD") )
        {
            while (reader->readNextStartElement() )
            {
                qCDebug(runtime)<<"adding element " << reader->name();
                contact[reader->name().toLatin1().toLower()] = QVariant(reader->readElementText());
            }
            return true;
        }
        else
        {
            reader->skipCurrentElement();
        }
    }
    return false;
}
