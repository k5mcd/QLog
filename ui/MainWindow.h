#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui/StatisticsWidget.h"
#include "core/SwitchButton.h"
#include "core/NetworkNotification.h"

namespace Ui {
class MainWindow;
}

class Conditions;
class QLabel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent* event);

signals:
    void settingsChanged();
    void themeChanged(int);

public slots:
    void rigErrorHandler(const QString &error);
    void rotErrorHandler(const QString &error);
    void stationProfileChanged();


private slots:
    void rigConnect();
    void rotConnect();
    void showSettings();
    void showStatistics();
    void importLog();
    void exportLog();
    void showLotw();
    void showeQSL();
    void showClublog();
    void showQRZ();
    void showAwards();
    void showAbout();
    void conditionsUpdated();
    void QSOFilterSetting();
    void darkModeToggle(int);

private:
    Ui::MainWindow* ui;
    Conditions* conditions;
    QLabel* conditionsLabel;
    QLabel* callsignLabel;
    QLabel* locatorLabel;
    QLabel* operatorLabel;
    SwitchButton* darkLightModeSwith;
    QLabel* darkIconLabel;
    StatisticsWidget* stats;
    NetworkNotification networknotification;


    void setDarkMode();
    void setLightMode();
};

#endif // MAINWINDOW_H
