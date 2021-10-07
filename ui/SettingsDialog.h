#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QHash>
#include <QSqlTableModel>

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
    void eqslDirBrowse();

private:
    void readSettings();
    void writeSettings();

    QSqlTableModel* modeTableModel;
    QSqlTableModel* bandTableModel;
    StationProfilesManager *profileManager;
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
