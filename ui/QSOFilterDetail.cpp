#include <QStringListModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTimeEdit>
#include <QStackedWidget>
#include "QSOFilterDetail.h"
#include "ui_QSOFilterDetail.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.qsofilterdetail");

QSOFilterDetail::QSOFilterDetail(QString filterName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QSOFilterDetail),
    filterName(filterName),
    condCount(0)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    logbookmodel = new LogbookModel(this);

    if ( ! filterName.isEmpty() )
    {
        loadFilter(filterName);
    }
    else
    {
        /* get Filters name from DB to checking whether a new filter name
         * will be unique */
        QSqlQuery filterStmt;
        if ( ! filterStmt.prepare("SELECT filter_name FROM qso_filters ORDER BY filter_name") )
        {
            qWarning() << "Cannot prepare select statement";
        }
        else
        {
            if ( filterStmt.exec() )
            {
                while (filterStmt.next())
                {
                    filterNamesList << filterStmt.value(0).toString();
                }
            }
            else
            {
                qInfo()<< "Cannot get filters names from DB" << filterStmt.lastError();;
            }
        }
    }
}

QSOFilterDetail::~QSOFilterDetail()
{
    FCT_IDENTIFICATION;
    delete ui;
}

void QSOFilterDetail::addCondition(int fieldIdx, int operatorId, QString value)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << "FieldIDX: " << fieldIdx << " Operator: " << operatorId << " Value: " << value;

    QHBoxLayout* conditionLayout = new QHBoxLayout();
    conditionLayout->setObjectName(QString::fromUtf8("conditionLayout%1").arg(condCount));

    /***************/
    /* Field Combo */
    /***************/
    QComboBox* fieldNameCombo = new QComboBox();
    fieldNameCombo->setObjectName(QString::fromUtf8("fieldNameCombo%1").arg(condCount));
    QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(fieldNameCombo->sizePolicy().hasHeightForWidth());
    fieldNameCombo->setSizePolicy(sizePolicy1);

    int columnIndex = 0;

    QStringList columnsNames;
    while ( columnIndex < logbookmodel->columnCount() )
    {
        columnsNames << logbookmodel->headerData(columnIndex, Qt::Horizontal).toString();
        columnIndex++;
    }

    QStringListModel* columnsNameModel = new QStringListModel(columnsNames,this);
    fieldNameCombo->setModel(columnsNameModel);

    if ( fieldIdx >= 0 )
    {
        fieldNameCombo->setCurrentIndex(fieldIdx);
    }

    conditionLayout->addWidget(fieldNameCombo);

    /*******************/
    /* Condition Combo */
    /*******************/
    QComboBox* conditionCombo = new QComboBox();
    conditionCombo->addItem(QString(tr("Equal")));
    conditionCombo->addItem(QString(tr("Not Equal")));
    conditionCombo->addItem(QString(tr("Contains")));
    conditionCombo->addItem(QString(tr("Not Contains")));
    conditionCombo->addItem(QString(tr("Greater Than")));
    conditionCombo->addItem(QString(tr("Less Than")));
    conditionCombo->setObjectName(QString::fromUtf8("conditionCombo%1").arg(condCount));

    if ( operatorId >= 0 )
    {
        conditionCombo->setCurrentIndex(operatorId);
    }

    conditionLayout->addWidget(conditionCombo);

    /**************/
    /* Value Edit */
    /**************/

    // Preparing Line Edit
    QLineEdit* valueEdit = new QLineEdit();
    valueEdit->setObjectName(QString::fromUtf8("valueLineEdit%1").arg(condCount));

    // Preparing Date Edit
    QDateEdit* valueDate = new QDateEdit();
    valueDate->setObjectName(QString::fromUtf8("valueDateEdit%1").arg(condCount));
    valueDate->setFocusPolicy(Qt::ClickFocus);
    valueDate->setCalendarPopup(true);
    valueDate->setTimeSpec(Qt::UTC);

    // Preparing DateTime Edit
    QDateTimeEdit* valueDateTime = new QDateTimeEdit();
    valueDateTime->setObjectName(QString::fromUtf8("valueDateTimeEdit%1").arg(condCount));
    valueDateTime->setFocusPolicy(Qt::ClickFocus);
    valueDateTime->setCalendarPopup(true);
    valueDateTime->setTimeSpec(Qt::UTC);

    // use stack to change Line and Date Edit - it will depend on column from combo selection
    QStackedWidget* stacked = new QStackedWidget();
    stacked->setObjectName(QString::fromUtf8("stackedValueEdit%1").arg(condCount));
    stacked->setMaximumSize(QSize(16777215, 30));
    stacked->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    stacked->addWidget(valueEdit);
    stacked->addWidget(valueDate);
    stacked->addWidget(valueDateTime);

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    stacked->setSizePolicy(sizePolicy);
    valueEdit->setSizePolicy(sizePolicy);
    valueDate->setSizePolicy(sizePolicy);
    valueDateTime->setSizePolicy(sizePolicy);

    conditionLayout->addWidget(stacked);

    if ( !value.isEmpty() )
    {
        if ( isDateField(fieldIdx) )
        {
            valueDate->setDate(QDate::fromString(value, "yyyy-MM-dd"));
            stacked->setCurrentIndex(1); //Date Edit

        }
        else if ( isDateTimeField(fieldIdx) )
        {
            valueDateTime->setDateTime(QDateTime::fromString(value, "yyyy-MM-ddTHH:mm"));
            stacked->setCurrentIndex(2); //DateTime Edit
        }
        else
        {
            valueEdit->setText(value);
            stacked->setCurrentIndex(0); //Date Edit
        }
    }


    // connect field combo and stacked widged to switch correct Edit Widget
    connect(fieldNameCombo, QOverload<int>::of(&QComboBox::activated),
            this, [this, stacked, value](int index)
    {
        /* Index is mapped the same way as LogbookModel columns
           Therefore, we can use Column aliases here
         */
        if ( this->isDateField(index) )
        {
            stacked->setCurrentIndex(1); //Date Edit
        }
        else if ( this->isDateTimeField(index) )
        {
            stacked->setCurrentIndex(2); //DateTime edit
        }
        else
        {
            stacked->setCurrentIndex(0); //Date Edit
        }
    });

    /*****************/
    /* Remove Button */
    /*****************/
    QPushButton* removeButton = new QPushButton(tr("Remove"));
    removeButton->setObjectName(QString::fromUtf8("removeButton%1").arg(condCount));

    conditionLayout->addWidget(removeButton);

    connect(removeButton, &QPushButton::clicked, this, [conditionLayout]()
    {
        QLayoutItem *item = NULL;
        while ((item = conditionLayout->takeAt(0)) != 0)
        {
            delete item->widget();
            delete item;
        }
        conditionLayout->deleteLater();
    });

    /**************************/
    /* Add to the main layout */
    /**************************/
    ui->conditionsLayout->addLayout(conditionLayout);

    condCount++;
}

void QSOFilterDetail::loadFilter(QString filterName)
{
    FCT_IDENTIFICATION;

    ui->filterLineEdit->setText(filterName);
    ui->filterLineEdit->setEnabled(false);

    QSqlQuery query;
    if ( ! query.prepare("SELECT matching_type, table_field_index, operator_id, value "
                  "FROM qso_filter_rules r, qso_filters f "
                  "WHERE f.filter_name = :filter AND f.filter_name = r.filter_name") )
    {
        qWarning() << "Cannot prepare select statement";
        return;
    }

    query.bindValue(":filter", filterName);

    if ( query.exec() )
    {
        while ( query.next() )
        {
            QSqlRecord record = query.record();

            ui->matchingCombo->setCurrentIndex(record.value("matching_type").toInt());
            addCondition(record.value("table_field_index").toInt(),
                         record.value("operator_id").toInt(),
                         record.value("value").toString());
        }
    }
    else
    {
        qCDebug(runtime) << "SQL execution error: " << query.lastError().text();
    }
}

bool QSOFilterDetail::filterExists(QString filterName)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << filterName;

    return filterNamesList.contains(filterName);
}

bool QSOFilterDetail::isDateField(int index)
{
    FCT_IDENTIFICATION;

    bool ret = (    index == LogbookModel::COLUMN_QSL_RCVD_DATE
                 || index == LogbookModel::COLUMN_QSL_SENT_DATE
                 || index == LogbookModel::COLUMN_LOTW_RCVD_DATE
                 || index == LogbookModel::COLUMN_LOTW_SENT_DATE
                 || index == LogbookModel::COLUMN_CLUBLOG_QSO_UPLOAD_DATE
                 || index == LogbookModel::COLUMN_EQSL_QSLRDATE
                 || index == LogbookModel::COLUMN_EQSL_QSLSDATE
                 || index == LogbookModel::COLUMN_HRDLOG_QSO_UPLOAD_DATE );

    qCDebug(function_parameters) << index << " return " << ret;
    return ret;
}

bool QSOFilterDetail::isDateTimeField(int index)
{
    FCT_IDENTIFICATION;

    bool ret = (    index == LogbookModel::COLUMN_TIME_ON
                 || index == LogbookModel::COLUMN_TIME_OFF );

    qCDebug(function_parameters) << index << " return " << ret;
    return ret;
}

void QSOFilterDetail::save()
{
    FCT_IDENTIFICATION;

    QString valueString;
    QSqlQuery filterInsertStmt, deleteFilterStmt, updateStmt;

    if ( ui->filterLineEdit->text().isEmpty() )
    {
        ui->filterLineEdit->setPlaceholderText(tr("Must not be empty"));
        return;
    }

    if ( filterExists(ui->filterLineEdit->text()) )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Info"),
                              QMessageBox::tr("Filter name is already exists."));
        return;
    }

    if ( ! filterInsertStmt.prepare("INSERT INTO qso_filter_rules(filter_name, table_field_index, operator_id, value) "
                              "VALUES (:filterName, :tableFieldIndex, :operatorID, :valueString)") )
    {
        qWarning() << "cannot preapre insert statement";
        return;
    }

    if ( ! updateStmt.prepare("INSERT INTO qso_filters (filter_name, matching_type) VALUES (:filterName, :matchingType) "
                       "ON CONFLICT(filter_name) DO UPDATE SET matching_type = :matchingType WHERE filter_name = :filterName") )
    {
        qWarning() << "Cannot prepare insert statement";
        return;
    }

    updateStmt.bindValue(":matchingType", ui->matchingCombo->currentIndex());
    updateStmt.bindValue(":filterName", ui->filterLineEdit->text());

    if ( ! deleteFilterStmt.prepare("DELETE FROM qso_filter_rules WHERE filter_name = :filterName") )
    {
        qWarning() << "Cannot prepare delete statement";
        return;
    }
    deleteFilterStmt.bindValue(":filterName", ui->filterLineEdit->text());

    QSqlDatabase::database().transaction();

    if ( updateStmt.exec() )
    {
        if ( deleteFilterStmt.exec() )
        {
            QList<QHBoxLayout *> conditionLayouts = ui->conditionsLayout->findChildren<QHBoxLayout *>();

            for (auto &condition: qAsConst(conditionLayouts) )
            {
                int fieldNameIdx = 0;
                int conditionIdx = 0;

                for (int i = 0; i < 3; i++)
                {

                    QString objectName = condition->itemAt(i)->widget()->objectName();

                    if ( objectName.contains("fieldNameCom") )
                    {
                        fieldNameIdx = dynamic_cast<QComboBox*>(condition->itemAt(i)->widget())->currentIndex();
                    }

                    if ( objectName.contains("conditionCombo") )
                    {
                        conditionIdx = dynamic_cast<QComboBox*>(condition->itemAt(i)->widget())->currentIndex();
                    }

                    if ( objectName.contains("stackedValueEdit") )
                    {
                        QStackedWidget* editStack = dynamic_cast<QStackedWidget*>(condition->itemAt(i)->widget());

                        QWidget* stackedEdit = editStack->currentWidget();

                        if ( stackedEdit )
                        {
                            QString stacketEditObjName = stackedEdit->objectName();

                            if ( stacketEditObjName.contains("valueLineEdit") )
                            {
                                QLineEdit* editLine = dynamic_cast<QLineEdit*>(stackedEdit);
                                valueString = editLine->text();
                                if ( valueString.isEmpty() )
                                {
                                    editLine->setPlaceholderText(tr("Must not be empty"));
                                    return;
                                }
                            }
                            else if ( stacketEditObjName.contains("valueDateEdit") )
                            {
                                QDateEdit* dateTimeEdit = dynamic_cast<QDateEdit*>(stackedEdit);
                                valueString = dateTimeEdit->date().toString(Qt::ISODate);
                            }
                            else if ( stacketEditObjName.contains("valueDateTimeEdit") )
                            {
                                QDateTimeEdit* dateEdit = dynamic_cast<QDateTimeEdit*>(stackedEdit);
                                valueString = dateEdit->dateTime().toString("yyyy-MM-ddTHH:mm");
                            }
                        }
                        else
                        {
                            qCCritical(runtime) << "Unexpected empty Stack - null pointer";
                        }
                    }
                }

                qCDebug(runtime)<< "Condition Values: " << ui->filterLineEdit->text() << " " << fieldNameIdx << " " << conditionIdx << " " << valueString;
                filterInsertStmt.bindValue(":filterName", ui->filterLineEdit->text());
                filterInsertStmt.bindValue(":tableFieldIndex", fieldNameIdx);
                filterInsertStmt.bindValue(":operatorID", conditionIdx);
                filterInsertStmt.bindValue(":valueString", valueString);

                if ( ! filterInsertStmt.exec() )
                {
                    QMessageBox::critical(nullptr, QMessageBox::tr("QLog Error"),
                                          QMessageBox::tr("Cannot update QSO Filter Conditions - ") + filterInsertStmt.lastError().text());
                    qInfo()<< "Cannot update QSO Filter Conditions - " << filterInsertStmt.lastError().text();
                    QSqlDatabase::database().rollback();
                    return;
                }
            }
            QSqlDatabase::database().commit();
        }
        else
        {
            QMessageBox::critical(nullptr, QMessageBox::tr("QLog Error"),
                                  QMessageBox::tr("Cannot delete QSO Filter Conditions before updating - ") + deleteFilterStmt.lastError().text());
            qInfo()<< "Cannot delete QSO Filter Conditions before updating -  " << deleteFilterStmt.lastError().text();
            QSqlDatabase::database().rollback();
            return;
        }
    }
    else
    {
        QMessageBox::critical(nullptr, QMessageBox::tr("QLog Error"),
                              QMessageBox::tr("Cannot Update QSO Filter Matching type - ") + updateStmt.lastError().text());
        qInfo()<< "Cannot Update QSO Filter Matching type - " << updateStmt.lastError().text();
        QSqlDatabase::database().rollback();
        return;
    }

    accept();
}

void QSOFilterDetail::filterNameChanged(const QString &newFilterName)
{
    FCT_IDENTIFICATION;

    if ( filterExists(newFilterName) )
    {
        ui->filterLineEdit->setStyleSheet("QLineEdit { color: red;}");
    }
    else
    {
        ui->filterLineEdit->setStyleSheet("QLineEdit { color: black;}");
    }

}
