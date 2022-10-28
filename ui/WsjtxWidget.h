#ifndef WSJTXWIDGET_H
#define WSJTXWIDGET_H

#include <QWidget>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include "data/WsjtxEntry.h"
#include "models/WsjtxTableModel.h"

namespace Ui {
class WsjtxWidget;
}

class WsjtxWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WsjtxWidget(QWidget *parent = nullptr);
    ~WsjtxWidget();

public slots:
    void decodeReceived(WsjtxDecode);
    void statusReceived(WsjtxStatus);
    void tableViewDoubleClicked(QModelIndex);
    void tableViewClicked(QModelIndex);
    void setSelectedCallsign(const QString&);

private slots:
    void displayedColumns();

signals:
    void showDxDetails(QString callsign, QString grid);
    void reply(WsjtxDecode);
    void CQSpot(WsjtxEntry);

private:
    WsjtxTableModel* wsjtxTableModel;
    WsjtxStatus status;
    QString band;
    double currFreq;
    QString currMode;
    Ui::WsjtxWidget *ui;
    QSortFilterProxyModel *proxyModel;
    QString lastSelectedCallsign;

    void saveTableHeaderState();
    void restoreTableHeaderState();
};

#endif // WSJTXWIDGET_H
