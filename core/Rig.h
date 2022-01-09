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

    void setFrequency(double);
    void setMode(const QString &, const QString &);
    void setPower(double);

signals:
    void frequencyChanged(double);
    void modeChanged(QString, QString);
    void powerChanged(double);
    void rigErrorPresent(QString);

private:
    Rig(QObject *parent = nullptr);

    Rig(Rig const&);
    ~Rig();
    void operator=(Rig const&);

    void __closeRig();
    RIG* rig;
    int freq_rx;
    rmode_t modeId;
    unsigned int power;
    QMutex rigLock;
    QTimer* timer;
};

#endif // RIG_H
