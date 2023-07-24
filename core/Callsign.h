#ifndef CALLSIGN_H
#define CALLSIGN_H

#include <QObject>
#include <QRegularExpression>

class Callsign : public QObject
{
    Q_OBJECT
public:
    explicit Callsign(const QString &callsign,
                      QObject *parent = nullptr);
    static const QRegularExpression callsignRegEx();
    static const QString callsignRegExString();
    static const QStringList secondarySpecialSuffixes;

    QString getCallsign() const;
    QString getHostPrefix() const;
    QString getHostPrefixWithDelimiter() const;
    QString getBase() const;
    QString getBasePrefix() const;
    QString getSuffix() const;
    QString getSuffixWithDelimiter() const;
    bool isValid() const;

private:
    QString fullCallsign;
    QString hostPrefix;
    QString hostPrefixWithDelimiter;
    QString base;
    QString basePrefix;
    QString suffix;
    QString suffixWithDelimiter;
    bool valid;
};

#endif // CALLSIGN_H
