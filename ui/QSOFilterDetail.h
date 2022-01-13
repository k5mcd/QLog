#ifndef QSOFILTERDETAIL_H
#define QSOFILTERDETAIL_H

#include <QDialog>
#include <QHBoxLayout>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTimeEdit>
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
    bool isQSLSentField(int index);
    bool isQSLSentViaField(int index);
    bool isQSLRcvdField(int index);
    bool isUploadStatusField(int index);
    bool isAntPathField(int index);
    bool isBoolField(int index);
    bool isQSOCompleteField(int index);
    QComboBox* createComboBox(const QMap<QString, QString>&, const QString&,
                              const int identifier, const QSizePolicy&);
    QDateEdit* createDateEdit(const QString&, const int, const QSizePolicy&);
    QDateTimeEdit* createDateTimeEdit(const QString&, const int, const QSizePolicy&);
    QLineEdit* createLineEdit(const QString&, const int, const QSizePolicy&);
};

#endif // QSOFILTERDETAIL_H
