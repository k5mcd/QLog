#ifndef CWCONSOLEWIDGET_H
#define CWCONSOLEWIDGET_H

#include <QWidget>
#include "ui/NewContactWidget.h"

namespace Ui {
class CWConsoleWidget;
}

class CWConsoleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CWConsoleWidget(QWidget *parent = nullptr);
    ~CWConsoleWidget();
    void registerContactWidget(const NewContactWidget*);

signals:
    void cwKeyProfileChanged();
    void cwShortcutProfileChanged();

public slots:
    void appendCWEchoText(QString);
    void reloadSettings();
    void clearConsoles();
    void setWPM(qint32);

private slots:
    void cwKeyProfileComboChanged(QString);
    void cwShortcutProfileComboChanged(QString);
    void cwShortcutProfileIncrease();
    void cwShortcutProfileDecrease();
    void refreshKeyProfileCombo();
    void refreshShortcutProfileCombo();
    void cwKeyConnected(QString);
    void cwKeyDisconnected();
    void cwKeySpeedChanged(int);
    void cwKeySpeedIncrease();
    void cwKeySpeedDecrease();
    void cwSendButtonPressed();
    void rigDisconnectHandler();
    void rigConnectHandler();

    void cwKeyMacroF1();
    void cwKeyMacroF2();
    void cwKeyMacroF3();
    void cwKeyMacroF4();
    void cwKeyMacroF5();
    void cwKeyMacroF6();
    void cwKeyMacroF7();
    void haltButtonPressed();


private:
    Ui::CWConsoleWidget *ui;
    bool cwKeyOnline;
    const NewContactWidget *contact;

    void sendCWText(const QString &);
    void expandMacros(QString &);
    void shortcutComboMove(int);
    void allowMorseSending(bool);

};

#endif // CWCONSOLEWIDGET_H
