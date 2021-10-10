#ifndef ROTATOR_H
#define ROTATOR_H

#include <QtCore>
#include <hamlib/rotator.h>

struct rot;

class Rotator : public QObject
{
    Q_OBJECT

public:
    static Rotator* instance();

signals:
    void positionChanged(int azimuth, int elevation);
    void rotErrorPresent(QString);

public slots:
    void start();
    void update();
    void open();
    void close();

    void setPosition(int azimuth, int elevation);

private:
    Rotator();

    void __closeRot();

    int azimuth;
    int elevation;

    ROT* rot;
    QMutex rotLock;
};

#endif // ROTATOR_H
