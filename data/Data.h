#ifndef DATA_H
#define DATA_H

#include <QtCore>
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

    static DxccStatus dxccStatus(int dxcc, QString band, QString mode);
    static Band band(double freq);
    static QString freqToMode(double freq);
    static QColor statusToColor(DxccStatus status, QColor defaultColor);
    static QColor statusToInverseColor(DxccStatus status, QColor defaultColor);
    static QString statusToText(DxccStatus status);
    QStringList contestList() { return contests.values(); }
    QStringList propagationModesList() { return propagationModes.values(); }
    QStringList propagationModesIDList() { return propagationModes.keys(); }
    QString propagationModeTextToID(QString propagationText) { return propagationModes.key(propagationText);}
    QString propagationModeIDToText(QString propagationID) { return propagationModes.value(propagationID);}
    DxccEntity lookupDxcc(QString callsign);
    QString dxccFlag(int dxcc);
    QPair<QString, QString> legacyMode(QString mode);
    QStringList satModeList() { return satModes.values();}
    QStringList satModesIDList() { return satModes.keys(); }
    QString satModeTextToID(QString satModeText) { return satModes.key(satModeText);}
    QStringList iotaList() { return iotaRef.values();}
    QStringList iotaIDList() { return iotaRef.keys();}
    QString iotaTextToID(QString iotaText) { return iotaRef.key(iotaText);}
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
