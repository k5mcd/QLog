#ifndef RIG_H
#define RIG_H

#include <QtCore>
#include <hamlib/rig.h>
#include "data/RigProfile.h"
#include "core/SerialPort.h"

#define QSTRING_FREQ(f) (QString::number((f), 'f', 5))
#define Hz2MHz(f) ((double)((f)/1e6))
#define mW2W(f) ((double)((f)/1000.0))

struct rig;

enum VFOID
{
    VFO1 = 0
};

Q_DECLARE_METATYPE(VFOID)

class LocalOscilator : public QObject
{
    Q_OBJECT

public:
    explicit LocalOscilator(VFOID id, QObject *parent = nullptr);
    ~LocalOscilator() {};

    freq_t getFreq() const;
    void setFreq(const freq_t &value);

    rmode_t getMode() const;
    QString getModeText() const;
    QString getModeNormalizedText(QString &submode) const;
    void setMode(const rmode_t &value);

    vfo_t getVFO() const;
    void setVFO(const vfo_t &value);
    QString getVFOText() const;

    unsigned int getPower() const;
    void setPower(unsigned int value);

    VFOID getID() const;
    void setID(VFOID);

    double getRXOffset() const;
    void setRXOffset(double value);

    double getTXOffset() const;
    void setTXOffset(double value);

    double getRITFreq() const;
    double getXITFreq() const;

    bool getPTT() const;
    void setPTT(bool);

    unsigned int getKeySpeed() const;
    void setKeySpeed(unsigned int value);

    pbwidth_t getPassbandWidth();
    void setPassbandWidth(pbwidth_t);

    void clear();

private:
    freq_t freq;          //in Hz
    rmode_t mode;
    vfo_t vfo;
    bool ptt;
    unsigned int power;   //in mW
    double RXOffset;      //in Hz
    double TXOffset;      //in Hz
    VFOID ID;
    pbwidth_t passbandWidth;
    unsigned int keySpeed;
};

class Rig : public SerialPort
{
    Q_OBJECT

public:
    static Rig* instance();

    static bool isNetworkRig(const struct rig_caps *caps);
    static qint32 getNormalBandwidth(const QString &, const QString &);
    bool isRigConnected();
    bool isMorseOverCatSupported();

public slots:
    void start();
    void update();
    void open();
    void close();
    void stopTimer();
    void sendState();

    void setFrequency(double);
    void setMode(const QString &, const QString &);
    void setMode(const QString &);
    void setMode(rmode_t);
    void setPTT(bool);
    void setKeySpeed(qint16 wpm);
    void syncKeySpeed(qint16 wpm);
    void sendMorse(const QString &text);
    void stopMorse();
    QStringList getAvailableModes();

signals:
    void frequencyChanged(VFOID, double, double, double);
    void modeChanged(VFOID, QString, QString, QString, qint32);
    void powerChanged(VFOID, double);
    void keySpeedChanged(VFOID, unsigned int);
    void vfoChanged(VFOID, QString);
    void ritChanged(VFOID, double);
    void xitChanged(VFOID, double);
    void pttChanged(VFOID, bool);
    void rigErrorPresent(QString, QString);
    void rigDisconnected();
    void rigConnected();
    void rigCWKeyOpenRequest(QString);
    void rigCWKeyCloseRequest(QString);

private slots:
    void openImpl();
    void closeImpl();
    void setFrequencyImpl(double);
    void setModeImpl(rmode_t);
    void setPTTImpl(bool);
    void stopTimerImplt();
    void setKeySpeedImpl(qint16 wpm);
    void syncKeySpeedImpl(qint16 wpm);
    void sendMorseImpl(const QString &text);
    void stopMorseImpl();

private:
    Rig(QObject *parent = nullptr);
    //Rig(Rig const&);
    ~Rig();

    void operator=(Rig const&);

    void __closeRig();
    void __openRig();
    void __setKeySpeed(qint16 wpm);

    static rmode_t modeSubmodeToModeT(const QString &mode, const QString &submode);
    QString hamlibErrorString(int);

    RIG* rig;
    RigProfile connectedRigProfile;

    LocalOscilator LoA;

    QMutex rigLock;
    QTimer* timer;

    bool forceSendState;
};

Q_DECLARE_METATYPE(rmode_t);

#endif // RIG_H
