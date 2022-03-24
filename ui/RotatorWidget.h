#ifndef ROTATORWIDGET_H
#define ROTATORWIDGET_H

#include <QWidget>
#include <QGraphicsPixmapItem>

namespace Ui {
class RotatorWidget;
}

class QGraphicsScene;
class QGraphicsPathItem;

class RotatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RotatorWidget(QWidget *parent = nullptr);
    ~RotatorWidget();

signals:
    void rotProfileChanged();

public slots:
    void gotoPosition();
    void positionChanged(int, int);
    void redrawMap();
    void rotProfileComboChanged(QString);
    void reloadSettings();

protected:
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);


private:
    void refreshRotProfileCombo();

    QGraphicsPathItem* compassNeedle;
    QGraphicsScene* compassScene;
    Ui::RotatorWidget *ui;
    int azimuth;
};

#endif // ROTATORWIDGET_H
