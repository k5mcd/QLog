#ifndef LOOKBOOKWIDGET_H
#define LOOKBOOKWIDGET_H

#include <QWidget>
#include "models/SqlListModel.h"

namespace Ui {
class LogbookWidget;
}

class ClubLog;
class LogbookModel;

class LogbookWidget : public QWidget {
    Q_OBJECT

public:
    explicit LogbookWidget(QWidget *parent = nullptr);
    ~LogbookWidget();

public slots:
    void filterCallsign(QString call);
    void filterSelectedCallsign();
    void lookupSelectedCallsign();
    void callsignFilterChanged();
    void bandFilterChanged();
    void modeFilterChanged();
    void countryFilterChanged();
    void userFilterChanged();
    void updateTable();
    void uploadClublog();
    void deleteContact();
    void editContact();
    void displayedColumns();
    void saveTableHeaderState();
    void showTableHeaderContextMenu(const QPoint& point);
    void doubleClickColumn(QModelIndex);


private:
    ClubLog* clublog;
    LogbookModel* model;
    Ui::LogbookWidget *ui;
    SqlListModel* countryModel;
    SqlListModel* userFilterModel;
};

#endif // LOGBOOKWIDGET_H
