#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QSqlTableModel>
#include <QCompleter>

#include "data/StationProfile.h"

namespace Ui {
class SettingsDialog;
}

class QSqlTableModel;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

public slots:
    void save();
    void addRig();
    void deleteRig();
    void addAnt();
    void deleteAnt();
    void refreshStationProfilesView();
    void addStationProfile();
    void deleteStationProfile();
    void doubleClickStationProfile(QModelIndex);
    void rigChanged(int);
    void rotChanged(int);
    void tqslPathBrowse();
    void adjustCallsignTextColor();
    void adjustLocatorTextColor();
    void adjustVUCCLocatorTextColor();
    void eqslDirBrowse();
    void cancelled();
    void sotaChanged(QString);
    void primaryCallbookChanged(int);
    void secondaryCallbookChanged(int);

private:
    void readSettings();
    void writeSettings();

    QSqlTableModel* modeTableModel;
    QSqlTableModel* bandTableModel;
    StationProfilesManager *profileManager;
    QCompleter *sotaCompleter;
    QCompleter *iotaCompleter;
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
