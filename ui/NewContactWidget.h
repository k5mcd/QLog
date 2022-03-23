#ifndef NEWCONTACTWIDGET_H
#define NEWCONTACTWIDGET_H

#include <QWidget>
#include <QSqlRecord>
#include <QCompleter>
#include "core/HamQTH.h"
#include "core/Cty.h"
#include "data/Data.h"
#include "core/Conditions.h"
#include "core/Gridsquare.h"
#include "data/DxSpot.h"

namespace Ui {
class NewContactWidget;
}

class HamQTH;
class Rig;

enum CoordPrecision {
    COORD_NONE = 0,
    COORD_DXCC = 1,
    COORD_GRID = 2,
    COORD_FULL = 3
};

class NewContactWidget : public QWidget {
    Q_OBJECT

public:
    explicit NewContactWidget(QWidget *parent = nullptr);
    ~NewContactWidget();

signals:
    void contactAdded(QSqlRecord record);
    void newTarget(double lat, double lon);
    void filterCallsign(QString call);
    void userFrequencyChanged(double freq);
    void stationProfileChanged();
    void rigProfileChanged();
    void markQSO(DxSpot spot);

public slots:
    void reloadSettings();
    void callsignChanged();
    void frequencyChanged();
    void frequencyRXChanged();
    void bandChanged();
    void modeChanged();
    void subModeChanged();
    void updateBand(double freq);
    void updateRXBand(double freq);
    void resetContact();
    void saveContact();
    void saveExternalContact(QSqlRecord record);
    void gridChanged();
    void updateTime();
    void updateTimeOff();
    void updateTimeStop();
    void startContactTimer();
    void stopContactTimer();
    void markContact();
    void editCallsignFinished();
    void callsignResult(const QMap<QString, QString>& data);
    void updateCoordinates(double lat, double lon, CoordPrecision prec);
    void updateDxccStatus();
    void changeFrequency(double freq);
    void changeMode(QString mode, QString subMode);
    void changePower(double power);
    void tuneDx(QString callsign, double frequency);
    void showDx(QString callsign, QString grid);
    void setDefaultReport();
    void qrz();
    void addPropConditions(Conditions *);
    void propModeChanged(const QString&);
    void rigFreqOffsetChanged(double);
    void rigFreqRXOffsetChanged(double);
    void stationProfileComboChanged(QString);
    void rigProfileComboChanged(QString);
    void sotaChanged(QString);
    void callbookCallsignNotFound(QString);


private:
    void queryDatabase(QString callsign);
    void queryDxcc(QString callsign);
    void clearQueryFields();
    void readSettings();
    void writeSettings();
    void __modeChanged();
    void refreshStationProfileCombo();
    void refreshRigProfileCombo();
    void addAddlFields(QSqlRecord &record);
    GenericCallbook *createCallbook(const QString&);

private:
    Rig* rig;
    QString callsign;
    DxccEntity dxccEntity;
    QString defaultReport;
    GenericCallbook *primaryCallbook;
    GenericCallbook *secondaryCallbook;
    QTimer* contactTimer;
    Ui::NewContactWidget *ui;
    CoordPrecision coordPrec;
    Conditions *prop_cond;
    QCompleter *iotaCompleter;
    QCompleter *satCompleter;
    QCompleter *sotaCompleter;
    double realRigFreq;
};

#endif // NEWCONTACTWIDGET_H
