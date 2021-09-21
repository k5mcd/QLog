#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

public slots:
    void rigErrorHandler(QString error);
    void rotErrorHandler(QString error);
    void stationProfileChanged();

private slots:
    void rigConnect();
    void rotConnect();
    void showSettings();
    void showStatistics();
    void importLog();
    void exportLog();
    void showLotw();
    void showAbout();
    void conditionsUpdated();

private:
    Ui::MainWindow* ui;
    Conditions* conditions;
    QLabel* conditionsLabel;
    QLabel* callsignLabel;
    QLabel* locatorLabel;
    QLabel* operatorLabel;
};

#endif // MAINWINDOW_H
