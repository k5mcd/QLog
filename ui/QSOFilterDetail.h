#ifndef QSOFILTERDETAIL_H
#define QSOFILTERDETAIL_H

#include <QDialog>
#include <QHBoxLayout>
#include "models/LogbookModel.h"

namespace Ui {
class QSOFilterDetail;
}

class QSOFilterDetail : public QDialog
{
    Q_OBJECT

public:
    explicit QSOFilterDetail(QString filterName = QString(), QWidget *parent = nullptr);
    ~QSOFilterDetail();

public slots:
    void addCondition(int fieldIdx = -1, int operatorId = -1, QString value = QString());
    void save();
    void filterNameChanged(const QString&);

private:
    Ui::QSOFilterDetail *ui;
    QString filterName;
    int condCount;
    LogbookModel* logbookmodel;
    QStringList filterNamesList;

private:
    void loadFilter(QString filterName);
    bool filterExists(QString filterName);
    bool isDateField(int index);
    bool isDateTimeField(int index);
};

#endif // QSOFILTERDETAIL_H
