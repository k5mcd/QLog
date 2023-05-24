#ifndef CWKEYER_H
#define CWKEYER_H

#include <QObject>

#include "core/CWKey.h"
#include "data/CWKeyProfile.h"

class CWKeyer : public QObject
{
    Q_OBJECT

public:
    static CWKeyer* instance();
    void stopTimer();

signals:
    void cwKeyerError(QString, QString);
    void cwKeyConnected(QString);
    void cwKeyDisconnected();
    void cwKeyWPMChanged(qint32);
    void cwKeyEchoText(QString);

public slots:
    void start();
    void update();
    void open();
    void close();
    bool canStopSending();
    bool canEchoChar();
    bool rigMustConnected();
    bool canSetSpeed();

    void setSpeed(const qint16 wpm);
    void sendText(const QString&);
    void imediatellyStop();

private slots:
    void openImpl();
    void closeImpl();
    void setSpeedImpl(const qint16 wpm);
    void sendTextImpl(const QString&);
    void immediatellyStopImpl();
    void stopTimerImplt();
    void keyErrorHandler(QString, QString);
    void cwKeyWPMChangedHandler(qint32);
    void cwKeyEchoTextHandler(QString);

private:
    explicit CWKeyer(QObject *parent = nullptr);
    ~CWKeyer();

    void __closeCWKey();
    void __openCWKey();

    CWKey *cwKey;
    CWKeyProfile connectedCWKeyProfile;
    QMutex cwKeyLock;
    QTimer* timer;
};

#endif // CWKEYER_H
