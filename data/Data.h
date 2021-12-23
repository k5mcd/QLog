#ifndef DATA_H
#define DATA_H

#include <QtCore>
#include <QRegularExpression>
#include "Dxcc.h"
#include "Band.h"

class Data : public QObject
{
    Q_OBJECT
public:
    static const QString MODE_CW;
    static const QString MODE_DIGITAL;
    static const QString MODE_FT8;
    static const QString MODE_LSB; // use just generic label SSB
    static const QString MODE_USB; // use just generic label SSB
    static const QString MODE_PHONE;

    explicit Data(QObject *parent = nullptr);
    static Data* instance();

    static DxccStatus dxccStatus(int dxcc, const QString &band, const QString &mode);
    static Band band(double freq);
    static QString freqToMode(double freq);
    static QString freqToBand(double freq);
    static QColor statusToColor(const DxccStatus &status, const QColor &defaultColor);
    static QColor statusToInverseColor(const DxccStatus &status, const QColor &defaultColor);
    static QString statusToText(const DxccStatus &status);
    static QRegularExpression callsignRegEx();
    static QString callsignRegExString();
    QStringList contestList() { return contests.values(); }
    QStringList propagationModesList() { return propagationModes.values(); }
    QStringList propagationModesIDList() { return propagationModes.keys(); }
    QString propagationModeTextToID(const QString &propagationText) { return propagationModes.key(propagationText);}
    QString propagationModeIDToText(const QString &propagationID) { return propagationModes.value(propagationID);}
    DxccEntity lookupDxcc(const QString &callsign);
    QString dxccFlag(int dxcc);
    QPair<QString, QString> legacyMode(const QString &mode);
    QStringList satModeList() { return satModes.values();}
    QStringList satModesIDList() { return satModes.keys(); }
    QString satModeTextToID(const QString &satModeText) { return satModes.key(satModeText);}
    QStringList iotaList() { return iotaRef.values();}
    QStringList iotaIDList() { return iotaRef.keys();}
    QString iotaTextToID(const QString &iotaText) { return iotaRef.key(iotaText);}
    QStringList sotaIDList() { return sotaRef.keys();}

signals:

public slots:

private:
    void loadContests();
    void loadPropagationModes();
    void loadLegacyModes();
    void loadDxccFlags();
    void loadSatModes();
    void loadIOTA();
    void loadSOTA();

    QMap<int, QString> flags;
    QMap<QString, QString> contests;
    QMap<QString, QString> propagationModes;
    QMap<QString, QPair<QString, QString>> legacyModes;
    QMap<QString, QString> satModes;
    QMap<QString, QString> iotaRef;
    QMap<QString, QString> sotaRef;
};

#endif // DATA_H
