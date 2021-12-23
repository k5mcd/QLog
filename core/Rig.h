#ifndef RIG_H
#define RIG_H

#include <QtCore>
#include <hamlib/rig.h>

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

    void setFrequency(double freq);
    void setMode(const QString &mod, const QString &submode);
    void setPower(double power);

signals:
    void frequencyChanged(double freq);
    void modeChanged(QString mode, QString submode);
    void powerChanged(double power);
    void rigErrorPresent(QString);

private:
    Rig(QObject *parent = nullptr);

    Rig(Rig const&);
    void operator=(Rig const&);

    void __closeRig();
    RIG* rig;
    int freq_rx;
    rmode_t modeId;
    unsigned int power;
    QMutex rigLock;
};

#endif // RIG_H
