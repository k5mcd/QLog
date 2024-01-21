#ifndef QLOG_UI_MAPWIDGET_H
#define QLOG_UI_MAPWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

namespace Ui {
class MapWidget;
}

class MapWidget : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget* parent = nullptr);
    ~MapWidget();

public slots:
    void setTarget(double lat, double lon);
    void clear();
    void redraw();

protected:
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);

private:
    void redrawNightOverlay();
    void drawPoint(const QPoint &point);
    void drawLine(const QPoint &pointA, const QPoint &pointB);

    void pointToRad(const QPoint &point, double& lat, double& lon);
    void pointToCoord(const QPoint &point, double& lat, double& lon);
    QPoint radToPoint(const double lat, const double lon);
    QPoint coordToPoint(const double lat, const double lon);

    int sunSize = 20;

    QGraphicsPixmapItem* nightOverlay;
    QList<QGraphicsItem*> items;
    QGraphicsEllipseItem* sunItem;
    QGraphicsPathItem* terminatorItem;
    QGraphicsScene* scene;
};

#endif // QLOG_UI_MAPWIDGET_H
