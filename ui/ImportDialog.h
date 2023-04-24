#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
#include <QSqlRecord>
#include <logformat/LogFormat.h>
#include "data/StationProfile.h"
#include "core/LogLocale.h"

namespace Ui {
class ImportDialog;
}

class ImportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ImportDialog(QWidget *parent = 0);
    ~ImportDialog();

private slots:
    void browse();
    void toggleAll();
    void toggleComment();
    void runImport();
    void computeProgress(qint64 position);
    void stationProfileTextChanged(QString);
    void rigProfileTextChanged(QString);
    void toggleMyProfile();
    void toggleMyRig();

private:
    Ui::ImportDialog *ui;
    qint64 size;
    StationProfile selectedStationProfile;
    LogLocale locale;

    static LogFormat::duplicateQSOBehaviour showDuplicateDialog(QSqlRecord *, QSqlRecord *);
};

#endif // IMPORTDIALOG_H
