#include <QTcpSocket>
#include <QtXml>
#include <QtDebug>
#include "Fldigi.h"
#include "logformat/AdiFormat.h"
#include "debug.h"

MODULE_IDENTIFICATION("qlog.core.fldigi");

Fldigi::Fldigi(QObject *parent) :
    QTcpServer(parent)
{
    FCT_IDENTIFICATION;

    listen(QHostAddress::Any, 8421);
}

void Fldigi::incomingConnection(int socket) {
    FCT_IDENTIFICATION;

    QTcpSocket* sock = new QTcpSocket(this);
    connect(sock, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(discardClient()));
    sock->setSocketDescriptor(socket);
}

void Fldigi::readClient() {
    FCT_IDENTIFICATION;

    QTcpSocket* sock = (QTcpSocket*)sender();

    QString data = QString(sock->readAll());
    qCDebug(runtime) << data;

    int split = data.indexOf("\r\n\r\n", 0);
    data.remove(0, split+4);

    QXmlStreamReader xml(data);
    processMethodCall(sock, xml);

    sock->close();
    if (sock->state() == QTcpSocket::UnconnectedState) {
        delete sock;
    }
}

void Fldigi::discardClient() {
    FCT_IDENTIFICATION;

    QTcpSocket* sock = (QTcpSocket*)sender();
    sock->deleteLater();
}

void Fldigi::processMethodCall(QTcpSocket* sock, QXmlStreamReader& xml) {
    FCT_IDENTIFICATION;

    QByteArray response;

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNextStartElement();

        if (xml.name() == "methodCall") {
            qCDebug(runtime) << "method call";
        }
        if (xml.name() == "methodName") {
            QString method = xml.readElementText();

            if (method == "log.add_record") {
                QString param = parseParam(xml);
                if (!param.isEmpty()) {
                    response = addRecord(param);
                }
            }
            else if (method == "system.listMethods") {
                response = listMethods();
            }
        }
    }

    qCDebug(runtime) << response;

    QTextStream out(sock);

    if (!response.isEmpty()) {
        out << "HTTP/1.1 200 OK\r\n";
        out << "Content-Type: text/xml; charset=utf-8\r\n";
        out << "Content-Length: " << response.length() << "\r\n";
        out << "\r\n";
        out << response;
    }
    else {
        out << "HTTP/1.1 400 Internal Error\r\n";
    }
}

QString Fldigi::parseParam(QXmlStreamReader& xml) {
    FCT_IDENTIFICATION;

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNextStartElement();
        if (xml.name() == "value") {
            return xml.readElementText();
        }
    }

    return QString();
}

QByteArray Fldigi::listMethods() {
    FCT_IDENTIFICATION;

    QByteArray out;

    QXmlStreamWriter xml(&out);
    xml.writeStartDocument();
    xml.writeStartElement("methodResponse");
    xml.writeStartElement("params");
    xml.writeStartElement("param");
    xml.writeStartElement("value");
    xml.writeStartElement("array");
    xml.writeStartElement("data");

    xml.writeTextElement("value", "log.add_record");
    xml.writeTextElement("value", "system.listMethods");

    xml.writeEndDocument();

    return out;
}

QByteArray Fldigi::addRecord(QString data) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << data;

    QByteArray out;
    QSettings settings;

    QXmlStreamWriter xml(&out);
    xml.writeStartDocument();
    xml.writeStartElement("methodResponse");
    xml.writeStartElement("params");
    xml.writeStartElement("param");
    xml.writeStartElement("value");
    xml.writeEndDocument();

    QMap<QString, QString> defaults;
    defaults["my_grid"] = settings.value("station/grid").toString();

    QTextStream in(&data);
    AdiFormat adif(in);
    adif.setDefaults(defaults);
    adif.runImport();

    emit contactAdded();

    return out;
}
