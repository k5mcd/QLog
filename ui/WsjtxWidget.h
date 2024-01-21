#ifndef QLOG_UI_WSJTXWIDGET_H
#define QLOG_UI_WSJTXWIDGET_H

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
    void actionFilter();

signals:
    void showDxDetails(QString callsign, QString grid);
    void reply(WsjtxDecode);
    void CQSpot(WsjtxEntry);

private:
    uint dxccStatusFilterValue();
    QString contFilterRegExp();
    int getDistanceFilterValue();
    int getSNRFilterValue();
    QStringList dxMemberList();
    void reloadSetting();

    WsjtxTableModel* wsjtxTableModel;
    WsjtxStatus status;
    QString band;
    double currFreq;
    QString currMode;
    Ui::WsjtxWidget *ui;
    QSortFilterProxyModel *proxyModel;
    QString lastSelectedCallsign;
    QRegularExpression contregexp;
    int distanceFilter;
    int snrFilter;
    uint dxccStatusFilter;
    QSet<QString> dxMemberFilter;
    void saveTableHeaderState();
    void restoreTableHeaderState();
};

#endif // QLOG_UI_WSJTXWIDGET_H
