#ifndef ALERTWIDGET_H
#define ALERTWIDGET_H

#include <QWidget>
#include "data/UserAlert.h"
#include "models/AlertTableModel.h"

namespace Ui {
class AlertWidget;
}

class AlertWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlertWidget(QWidget *parent = nullptr);
    ~AlertWidget();

public slots:
    void addAlert(const UserAlert &alert);
    int alertCount() const;

private:
    Ui::AlertWidget *ui;
    AlertTableModel* alertTableModel;
};

#endif // ALERTWIDGET_H
