#ifndef CWKEY_H
#define CWKEY_H

#include <QObject>
#include <QtSerialPort>

class CWKeySerialInterface
{
public:
    explicit CWKeySerialInterface(const QString &portName,
                                  const qint32 baudrate,
                                  const qint32 timeout);
    ~CWKeySerialInterface() {};

protected:

    virtual qint64 sendDataAndWait(const QByteArray &data);
    virtual qint64 receiveDataAndWait(QByteArray &data);
    virtual qint64 writeAsyncData(const QByteArray &);

    QSerialPort serial;
    qint32 timeout;
    QMutex portMutex;
};

class CWKey : public QObject
{
    Q_OBJECT;
public:
    enum CWKeyTypeID
    {
        DUMMY_KEYER = 0,
        WINKEY2_KEYER = 1,
        LAST_MODEL = 2
    };

    enum CWKeyModeID
    {
        SINGLE_PADDLE = 0,
        IAMBIC_A = 1,
        IAMBIC_B = 2,
        ULTIMATE = 3,
        LAST_MODE = 4
    };

signals:
    void keyError(QString, QString);
    void keyChangedWPMSpeed(qint32);
    void keyEchoText(QString);

public:
    explicit CWKey(CWKeyModeID mode, qint32 defaultWPM, QObject *parent = nullptr);
    virtual ~CWKey() {};

    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool sendText(const QString &text) = 0;
    virtual bool setWPM(const qint16 wpm) = 0;
    virtual QString lastError() = 0;
    virtual bool imediatellyStop() = 0;

    static CWKeyTypeID intToTypeID(int);
    static CWKeyModeID intToModeID(int);

protected:
    CWKeyModeID keyMode;
    qint32 defaultWPMSpeed;
};

#endif // CWKEY_H
