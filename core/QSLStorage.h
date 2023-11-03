#ifndef QSLSTORAGE_H
#define QSLSTORAGE_H

#include <QObject>
#include <QSqlRecord>
#include <QVariant>

class QSLObject
{
public:
    enum SourceType
    {
        QSLFILE = 0,
        EQSL = 1,
    };

    enum BLOBFormat
    {
        BASE64FORM,
        RAWBYTES
    };

    explicit QSLObject( const qulonglong &qsoID,
                        const SourceType source,
                        const QString &qslName,
                        const QByteArray &inBlob,
                        const BLOBFormat format) :
        qsoID(qsoID),
        source(source),
        qslName(qslName),
        blob((format == RAWBYTES) ? inBlob : QByteArray::fromBase64(inBlob))
        {};

    explicit QSLObject( const QSqlRecord &qso,
                        const SourceType source,
                        const QString &qslName,
                        const QByteArray &inBlob,
                        const BLOBFormat format) :
        QSLObject(qso.value("id").toULongLong(),
                  source, qslName, inBlob, format)
    {}

    qulonglong getQSOID() const {return qsoID;};
    QSLObject::SourceType getSource() const {return source;};
    QString getQSLName() const {return qslName;};
    QByteArray getBLOB(BLOBFormat format = RAWBYTES) const {return (format == BASE64FORM) ? blob.toBase64() : blob;};

private:
    qulonglong qsoID;
    SourceType source;
    QString qslName;
    QByteArray blob;
};

class QSLStorage : public QObject
{
    Q_OBJECT

public:
    explicit QSLStorage(QObject *parent = nullptr);

    bool add(const QSLObject &);
    bool remove(const QSqlRecord &qso,
                const QSLObject::SourceType source,
                const QString &qslName);
    QStringList getAvailableQSLNames(const QSqlRecord &qso,
                                     const QSLObject::SourceType sourceFilter) const;
    QSLObject getQSL(const QSqlRecord &qso,
                     const QSLObject::SourceType source,
                     const QString &qslName) const;
};

#endif // QSLSTORAGE_H
