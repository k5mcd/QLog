#ifndef TCIDRV_H
#define TCIDRV_H

#include <QtWebSockets>
#include <QHash>
#include "GenericDrv.h"
#include "rig/RigCaps.h"

class TCIDrv : public GenericDrv
{
    Q_OBJECT

public:
    static QList<QPair<int, QString>> getModelList();
    static RigCaps getCaps(int);

    explicit TCIDrv(const RigProfile &profile,
                    QObject *parent = nullptr);
    virtual ~TCIDrv();

    virtual bool open() override;
    virtual bool isMorseOverCatSupported() override;
    virtual QStringList getAvailableModes() override;

    virtual void setFrequency(double) override;
    virtual void setRawMode(const QString &) override;
    virtual void setMode(const QString &, const QString &) override;
    virtual void setPTT(bool) override;
    virtual void setKeySpeed(qint16 wpm) override;
    virtual void syncKeySpeed(qint16 wpm) override;
    virtual void sendMorse(const QString &) override;
    virtual void stopMorse() override;
    virtual void sendState() override;
    virtual void stopTimers() override;
    virtual void sendDXSpot(const DxSpot &spot) override;

private slots:
    void onConnected();
    void onTextMessageReceived(const QString& message);
    void onSocketError(QAbstractSocket::SocketError socker_error);
private:

    typedef void (TCIDrv::*parseFce)(const QStringList&);

    void sendCmd(const QString &cmd,
                 bool addRigID,
                 const QStringList &args = QStringList());
    const QString getModeNormalizedText(const QString& rawMode, QString &submode);
    const QString mode2RawMode(const QString &mode, const QString &submode);

    // commands functions
    void rspPROTOCOL(const QStringList &);
    void rspREADY(const QStringList &);
    void rspSTART(const QStringList &);
    void rspSTOP(const QStringList &);
    void rspRECEIVE_ONLY(const QStringList &);
    void rspMODULATIONS_LIST(const QStringList &);
    void rspVFO(const QStringList &);
    void rspTRX(const QStringList &);
    void rspMODULATION(const QStringList &);
    void rspTUNE_DRIVE(const QStringList &);
    void rspDRIVE(const QStringList &);
    void rspRIT_OFFSET(const QStringList &);
    void rspXIT_OFFSET(const QStringList &);
    void rspCW_MACROS_SPEED(const QStringList &);
    void rspRIT_ENABLE(const QStringList &);
    void rspXIT_ENABLE(const QStringList &);

    double getRITFreq();
    void setRITFreq(double);
    double getXITFreq();
    void setXITFreq(double);
    double getRawRIT();
    double getRawXIT();

    QWebSocket ws;
    bool ready;
    bool receivedOnly;
    QStringList modeList;
    double currFreq;
    double currRIT;
    double currXIT;
    bool RITEnabled;
    bool XITEnabled;

    const QHash<QString, TCIDrv::parseFce> responseParsers =
    {
        {"protocol", &TCIDrv::rspPROTOCOL},
        {"ready", &TCIDrv::rspREADY},
        {"start", &TCIDrv::rspSTART},
        {"stop", &TCIDrv::rspSTOP},
        {"receive_only", &TCIDrv::rspRECEIVE_ONLY},
        {"modulations_list", &TCIDrv::rspMODULATIONS_LIST},
        {"vfo", &TCIDrv::rspVFO},
        {"trx", &TCIDrv::rspTRX},
        {"modulation", &TCIDrv::rspMODULATION},
        {"tune_drive", &TCIDrv::rspTUNE_DRIVE},
        {"drive", &TCIDrv::rspDRIVE},
        {"rit_offset", &TCIDrv::rspRIT_OFFSET},
        {"xit_offset", &TCIDrv::rspXIT_OFFSET},
        {"cw_macros_speed", &TCIDrv::rspCW_MACROS_SPEED},
        {"rit_enable", &TCIDrv::rspRIT_ENABLE},
        {"xit_enable", &TCIDrv::rspXIT_ENABLE},
    };
};

#endif // TCIDRV_H
