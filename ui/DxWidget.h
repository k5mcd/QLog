#ifndef DXWIDGET_H
#define DXWIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <QSortFilterProxyModel>
#include <QRegularExpression>

#include "data/Data.h"
#include "data/DxSpot.h"

namespace Ui {
class DxWidget;
}

class DxTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    DxTableModel(QObject* parent = 0) : QAbstractTableModel(parent) {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void addEntry(DxSpot entry);
    QString getCallsign(const QModelIndex& index);
    double getFrequency(const QModelIndex& index);
    void clear();

private:
    QList<DxSpot> dxData;
};

class DXSpotFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit DXSpotFilterProxyModel(QObject* parent= nullptr);

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const  override;

public slots:
    void setModeFilterRegExp(const QString& regExp);
    void setContFilterRegExp(const QString& regExp);
    void setSpotterContFilterRegExp(const QString& regExp);
    void setBandFilterRegExp(const QString& regExp);

private:
    QRegularExpression moderegexp;
    QRegularExpression contregexp;
    QRegularExpression spottercontregexp;
    QRegularExpression bandregexp;
};

class DeleteHighlightedDXServerWhenDelPressedEventFilter : public QObject
{
     Q_OBJECT
protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class DxWidget : public QWidget {
    Q_OBJECT

public:
    explicit DxWidget(QWidget *parent = 0);
    ~DxWidget();

public slots:
    void toggleConnect();
    void receive();
    void send();
    void connected();
    void socketError(QAbstractSocket::SocketError);
    void rawModeChanged();
    void entryDoubleClicked(QModelIndex);
    void actionFilter();
    void adjusteServerSelectSize(QString);
    void serverSelectChanged(int);

signals:
    void tuneDx(QString, double);
    void newSpot(DxSpot);

private:
    DxTableModel* dxTableModel;
    DXSpotFilterProxyModel *proxyDXC;
    QTcpSocket* socket;
    Ui::DxWidget *ui;

    void connectCluster();
    void disconnectCluster();
    void saveDXCServers();
    QString modeFilterRegExp();
    QString contFilterRegExp();
    QString spotterContFilterRegExp();
    QString bandFilterRegExp();

    QStringList getDXCServerList(void);
};

#endif // DXWIDGET_H
