#ifndef WSJTXWIDGET_H
#define WSJTXWIDGET_H

#include <QWidget>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include "core/Wsjtx.h"
#include "data/Data.h"

namespace Ui {
class WsjtxWidget;
}

struct WsjtxEntry {
    WsjtxDecode decode;
    DxccEntity dxcc;
    DxccStatus status;
    QString callsign;
    QString grid;
    QDateTime receivedTime;
};

class WsjtxTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    WsjtxTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent) {spotAgingPeriod = 120;}
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void addOrReplaceEntry(WsjtxEntry entry);
    void spotAging();
    bool callsignExists(WsjtxEntry call);
    QString getCallsign(QModelIndex idx);
    QString getGrid(QModelIndex idx);
    WsjtxDecode getDecode(QModelIndex idx);
    void setSpotAging(int seconds);

private:
    QList<WsjtxEntry> wsjtxData;
    int spotAgingPeriod;
};

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

signals:
    void showDxDetails(QString callsign, QString grid);
    void reply(WsjtxDecode);

private:
    WsjtxTableModel* wsjtxTableModel;
    WsjtxStatus status;
    QString band;
    Ui::WsjtxWidget *ui;
    QSortFilterProxyModel *proxyModel;
    QString lastSelectedCallsign;
};

#endif // WSJTXWIDGET_H
