#ifndef LOOKBOOKWIDGET_H
#define LOOKBOOKWIDGET_H

#include <QWidget>
#include <QProxyStyle>
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

signals:
    void logbookUpdated();
    void contactUpdated(QSqlRecord&);
    void contactDeleted(QSqlRecord&);

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
    void exportContact();
    void editContact();
    void displayedColumns();
    void saveTableHeaderState();
    void showTableHeaderContextMenu(const QPoint& point);
    void doubleClickColumn(QModelIndex);
    void handleBeforeUpdate(int, QSqlRecord&);
    void handleBeforeDelete(int);
    void focusSearchCallsign();

private:
    ClubLog* clublog;
    LogbookModel* model;
    Ui::LogbookWidget *ui;
    SqlListModel* countryModel;
    SqlListModel* userFilterModel;
};

/* https://forum.qt.io/topic/90403/show-tooltip-immediatly/7/ */
class ProxyStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;
    int styleHint(StyleHint hint, const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const override
    {
        if (hint == QStyle::SH_ToolTip_WakeUpDelay)
            return 0;  // show tooltip immediately
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

#endif // LOGBOOKWIDGET_H
