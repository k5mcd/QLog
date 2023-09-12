#include <QPushButton>
#include <QLabel>
#include <QTabBar>
#include <QMessageBox>
#include <QStandardItemModel>

#include "ChatWidget.h"
#include "ui_ChatWidget.h"
#include "core/debug.h"
#include "core/KSTChat.h"
#include "ui/KSTChatWidget.h"

MODULE_IDENTIFICATION("qlog.ui.chatwidget");

ChatWidget::ChatWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWidget)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    ui->chatTabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->hide();
    ui->chatRoomCombo->addItems(KSTChat::chatRooms);

    QWidget *w = ui->chatTabWidget->widget(0);
    w->setProperty("chatName", tr("New"));
    ui->chatTabWidget->setTabText(0, generateTabName(w));
    QSettings settings;
    ui->chatRoomCombo->setCurrentIndex(settings.value("chat/last_selected_room", 0).toInt());
}

ChatWidget::~ChatWidget()
{
    FCT_IDENTIFICATION;

    for (int i = 0; i < ui->chatTabWidget->count(); i++ )
    {
        if ( qobject_cast<KSTChatWidget*>(ui->chatTabWidget->widget(i)) )
            ui->chatTabWidget->widget(i)->disconnect(SIGNAL(chatClosed()));
    }
    delete ui;
}

void ChatWidget::registerContactWidget(const NewContactWidget *contactWidget)
{
    FCT_IDENTIFICATION;
    contact = contactWidget;
}

void ChatWidget::setChatCallsign(QString callsign)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << callsign;

    KSTChatWidget *kstWidget = qobject_cast<KSTChatWidget*>(ui->chatTabWidget->currentWidget());
    if ( kstWidget )
    {
        kstWidget->setPrivateChatCallsign(callsign);
    }
}

void ChatWidget::reloadStationProfile()
{
    FCT_IDENTIFICATION;

    for (int i = 0; i < ui->chatTabWidget->count(); i++ )
    {
        KSTChatWidget *kstWidget = qobject_cast<KSTChatWidget*>(ui->chatTabWidget->widget(i));

        if ( kstWidget )
            kstWidget->reloadStationProfile();
    }
}

void ChatWidget::connectChat()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    QString username = KSTChat::getUsername();
    QString password = KSTChat::getPassword();

    if ( username.isEmpty()
         || password.isEmpty() )
    {
        QMessageBox::warning(this, tr("QLog Warning"), tr("ON4KST Chat is not configured properly.<p> Please, use <b>Settings</b> dialog to configure it.</p>"));
        return;
    }

    KSTChatWidget *newWidget = new KSTChatWidget(ui->chatRoomCombo->currentIndex() + 1,
                                                 username,
                                                 password,
                                                 contact,
                                                 this);
    newWidget->setProperty("chatName", ui->chatRoomCombo->currentText());
    newWidget->setProperty("unread", 0);
    ui->chatTabWidget->addTab(newWidget,generateTabName(newWidget));
    ui->chatTabWidget->setCurrentWidget(newWidget);
    connect(newWidget, &KSTChatWidget::chatClosed,
            this, [this, newWidget]()
    {
        int i = this->findIndex(newWidget);
        if ( i >= 0 )
        {
            this->closeTab(i);
        }
    });

    connect(newWidget, &KSTChatWidget::chatUpdated,
            this, &ChatWidget::tabActive);

    connect(newWidget, &KSTChatWidget::chatQSOInfo,
            this, &ChatWidget::processQSOInfo);

    connect(newWidget, &KSTChatWidget::userListUpdated,
            this, &ChatWidget::userListUpdate);

    connect(newWidget, &KSTChatWidget::beamingRequested,
            this, &ChatWidget::beamRequest);

    // Disable Chat Room in the Combobox
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->chatRoomCombo->model());
    if ( model )
    {
        QStandardItem *item = model->item(ui->chatRoomCombo->currentIndex());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }
    settings.setValue("chat/last_selected_room", ui->chatRoomCombo->currentIndex());
}

void ChatWidget::closeTab(int tabIndex)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << tabIndex;

    if ( tabIndex < 0 )
        return;

    KSTChatWidget *kstWidget = qobject_cast<KSTChatWidget*>(ui->chatTabWidget->widget(tabIndex));
    ui->chatTabWidget->removeTab(tabIndex);

    // Enable Chat Room in the Combobox
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->chatRoomCombo->model());

    if ( model )
    {
        QStandardItem *item = model->item(ui->chatRoomCombo->findText(kstWidget->property("chatName").toString()));
        item->setFlags(item->flags() | Qt::ItemIsEnabled);
    }

    kstWidget->deleteLater();
    emit userListUpdated(QList<KSTUsersInfo>());
}

void ChatWidget::tabActive(QWidget *w)
{
    FCT_IDENTIFICATION;

    if ( w == ui->chatTabWidget->currentWidget() )
        return;

    int unread = w->property("unread").toInt();
    unread++;
    w->setProperty("unread", unread);

    int i = findIndex(w);
    if ( i >=0 )
        ui->chatTabWidget->setTabText(i, generateTabName(w));
}

void ChatWidget::chatTabClicked(int tabIndex)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << tabIndex;

    QWidget *w = ui->chatTabWidget->widget(tabIndex);
    w->setProperty("unread", 0);
    ui->chatTabWidget->setTabText(tabIndex, generateTabName(w));
    KSTChatWidget *kstWidget = qobject_cast<KSTChatWidget*>(w);
    if ( kstWidget )
        emit userListUpdated(kstWidget->getUserList());
    else
        emit userListUpdated(QList<KSTUsersInfo>());
}

void ChatWidget::processQSOInfo(QString callsign, QString grid)
{
    FCT_IDENTIFICATION;
    emit chatQSOInfo(callsign, grid);
}

void ChatWidget::userListUpdate(QWidget *w)
{
    FCT_IDENTIFICATION;

    if ( w != ui->chatTabWidget->currentWidget() )
        return;

    KSTChatWidget *kstWidget = qobject_cast<KSTChatWidget*>(w);
    if ( kstWidget )
        emit userListUpdated(kstWidget->getUserList());
}

void ChatWidget::beamRequest(double az)
{
    FCT_IDENTIFICATION;

    emit beamingRequested(az);
}

int ChatWidget::findIndex(QWidget *w)
{
    FCT_IDENTIFICATION;

    for (int i = 0 ; i < ui->chatTabWidget->count(); i++ )
    {
        if ( ui->chatTabWidget->widget(i) == w )
            return i;
    }

    return -1;
}

QString ChatWidget::generateTabName(QWidget *w)
{
    FCT_IDENTIFICATION;

    int unread = w->property("unread").toInt();

    return w->property("chatName").toString()
            + (( unread > 0 ) ? QString(" (") + QString::number(unread) + ")"
                              : "");
}
