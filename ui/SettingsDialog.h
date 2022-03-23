#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QSqlTableModel>
#include <QCompleter>

#include "data/StationProfile.h"
#include "data/RigProfile.h"
#include "data/RotProfile.h"
#include "data/AntProfile.h"

namespace Ui {
class SettingsDialog;
}

class QSqlTableModel;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

public slots:
    void save();

    void addRigProfile();
    void delRigProfile();
    void refreshRigProfilesView();
    void doubleClickRigProfile(QModelIndex);

    void addRotProfile();
    void delRotProfile();
    void refreshRotProfilesView();
    void doubleClickRotProfile(QModelIndex);

    void addAntProfile();
    void delAntProfile();
    void refreshAntProfilesView();
    void doubleClickAntProfile(QModelIndex);

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
    void paperDirBrowse();
    void cancelled();
    void sotaChanged(QString);
    void primaryCallbookChanged(int);
    void secondaryCallbookChanged(int);

private:
    void readSettings();
    void writeSettings();

    QSqlTableModel* modeTableModel;
    QSqlTableModel* bandTableModel;
    StationProfilesManager *stationProfManager;
    RigProfilesManager *rigProfManager;
    RotProfilesManager *rotProfManager;
    AntProfilesManager *antProfManager;
    QCompleter *sotaCompleter;
    QCompleter *iotaCompleter;
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
