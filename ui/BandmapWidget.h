#ifndef BANDMAPWIDGET_H
#define BANDMAPWIDGET_H

#include <QWidget>
#include <QMap>
#include <QTimer>
#include <QGraphicsItem>

#include "data/DxSpot.h"
#include "data/Band.h"
#include "core/Rig.h"

namespace Ui {
class BandmapWidget;
}

class QGraphicsScene;

class BandmapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BandmapWidget(QWidget *parent = nullptr);
    ~BandmapWidget();

    enum BandmapZoom {
        ZOOM_100HZ,
        ZOOM_250HZ,
        ZOOM_500HZ,
        ZOOM_1KHZ,
        ZOOM_2K5HZ,
        ZOOM_5KHZ,
        ZOOM_10KHZ
    };

public slots:
    void update();
    void updateRxFrequency(VFOID, double, double, double);
    void addSpot(DxSpot spot);
    void spotAgingChanged(int);
    void clearSpots();
    void zoomIn();
    void zoomOut();
    void spotClicked(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason);

signals:
    void tuneDx(QString, double);

private:
    void removeDuplicates(DxSpot &spot);
    void bandmapAging();

private:
    Ui::BandmapWidget *ui;



    double rx_freq;
    Band band;
    BandmapZoom zoom;
    QGraphicsScene* bandmapScene;
    QMap<double, DxSpot> spots;
    QTimer *update_timer;
};

#endif // BANDMAPWIDGET_H
