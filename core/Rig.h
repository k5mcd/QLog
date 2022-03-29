#ifndef RIG_H
#define RIG_H

#include <QtCore>
#include <hamlib/rig.h>
#include "data/RigProfile.h"

struct rig;

class Rig : public QObject {
    Q_OBJECT

public:
    static Rig* instance();

public slots:
    void start();
    void update();
    void open();
    void close();

    void setFrequency(double);
    void setMode(const QString &, const QString &);
    void setMode(const QString &);
    void setMode(rmode_t);
    QStringList getAvailableModes();

signals:
    void frequencyChanged(double);
    void modeChanged(QString, QString);
    void powerChanged(double);
    void vfoChanged(unsigned int);
    void rigErrorPresent(QString);
    void rigDisconnected();
    void rigConnected();

private:
    Rig(QObject *parent = nullptr);

    Rig(Rig const&);
    ~Rig();
    void operator=(Rig const&);

    void __closeRig();
    void __openRig();
    RIG* rig;
    RigProfile connectedRigProfile;
    int freq_rx;
    rmode_t modeId;
    vfo_t vfoId;
    unsigned int power;
    QMutex rigLock;
    QTimer* timer;
};

#endif // RIG_H
