#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include "core/KSTChat.h"
#include "ui/NewContactWidget.h"

namespace Ui {
class ChatWidget;
}

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    ~ChatWidget();

    void registerContactWidget(const NewContactWidget *contactWidget);

public slots:
    void setChatCallsign(QString);
    void reloadStationProfile();

private slots:
    void connectChat();
    void closeTab(int);
    void tabActive(QWidget *);
    void chatTabClicked(int);
    void processQSOInfo(QString, QString);
    void userListUpdate(QWidget *w);
    void beamRequest(double);

signals:
    void chatQSOInfo(QString, QString);
    void userListUpdated(QList<KSTUsersInfo>);
    void beamingRequested(double);

private:
    Ui::ChatWidget *ui;
    const NewContactWidget *contact;

    int findIndex(QWidget *);
    QString generateTabName(QWidget *);
};

#endif // CHATWIDGET_H
