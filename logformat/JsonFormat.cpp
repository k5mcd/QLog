#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlRecord>
#include "JsonFormat.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.logformat.jsonformat");

void JsonFormat::exportContact(const QSqlRecord& record, QMap<QString, QString>*)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<record;

    QJsonObject contact;
    int fieldCount = record.count();
    for (int i = 0; i < fieldCount; i++) {
        QString fieldName = record.fieldName(i);
        QVariant fieldValue = record.value(i);
        if (fieldValue.isNull()) continue;
        contact[fieldName] = QJsonValue::fromVariant(fieldValue);
    }
    data.append(contact);
}

void JsonFormat::exportEnd() {
    FCT_IDENTIFICATION;

    QJsonDocument doc(data);
    QByteArray json = doc.toJson();
    stream << json;
}

bool JsonFormat::importNext(QSqlRecord&) {
    return false;
}
