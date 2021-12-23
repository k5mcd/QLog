#include <QCheckBox>
#include <QPushButton>
#include "ColumnSettingDialog.h"
#include "ui_ColumnSettingDialog.h"
#include "models/LogbookModel.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.ColumnSettingDialog");

ColumnSettingDialog::ColumnSettingDialog(QTableView *table, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColumnSettingDialog),
    table(table)
{
    FCT_IDENTIFICATION;
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Done"));
    ui->tabWidget->setCurrentIndex(0);

    int checkboxesPerRow = 4;
    int columnIndex = 0;

    QList<QCheckBox*> generalCheckboxList;
    QList<QCheckBox*> myInfoCheckboxList;
    QList<QCheckBox*> qslInfoCheckboxList;
    QList<QCheckBox*> membersInfoCheckboxList;
    QList<QCheckBox*> otherInfoCheckboxList;
    QList<QCheckBox*> conditionsCheckboxList;
    QList<QCheckBox*> contestCheckboxList;

    while ( columnIndex < table->model()->columnCount() )
    {
        QCheckBox *columnCheckbox=new QCheckBox();
        QString columnNameString = table->model()->headerData(columnIndex, Qt::Horizontal).toString();

        columnCheckbox->setChecked(!table->isColumnHidden(columnIndex));
        columnCheckbox->setText(columnNameString);

        connect(columnCheckbox, &QCheckBox::stateChanged, this, [columnIndex, table]() {
            table->setColumnHidden(columnIndex, !table->isColumnHidden(columnIndex));
        });

        switch ( columnIndex )
        {
        case LogbookModel::COLUMN_MY_ANTENNA:
        case LogbookModel::COLUMN_MY_ANTENNA_INTL:
        case LogbookModel::COLUMN_MY_CITY:
        case LogbookModel::COLUMN_MY_CITY_INTL:
        case LogbookModel::COLUMN_MY_CNTY:
        case LogbookModel::COLUMN_MY_COUNTRY:
        case LogbookModel::COLUMN_MY_COUNTRY_INTL:
        case LogbookModel::COLUMN_MY_CQ_ZONE:
        case LogbookModel::COLUMN_MY_DXCC:
        case LogbookModel::COLUMN_MY_FISTS:
        case LogbookModel::COLUMN_MY_GRIDSQUARE:
        case LogbookModel::COLUMN_MY_IOTA:
        case LogbookModel::COLUMN_MY_IOTA_ISLAND_ID:
        case LogbookModel::COLUMN_MY_ITU_ZONE:
        case LogbookModel::COLUMN_MY_LAT:
        case LogbookModel::COLUMN_MY_LON:
        case LogbookModel::COLUMN_MY_NAME:
        case LogbookModel::COLUMN_MY_NAME_INTL:
        case LogbookModel::COLUMN_MY_POSTAL_CODE:
        case LogbookModel::COLUMN_MY_POSTAL_CODE_INTL:
        case LogbookModel::COLUMN_MY_RIG:
        case LogbookModel::COLUMN_MY_RIG_INTL:
        case LogbookModel::COLUMN_MY_SIG:
        case LogbookModel::COLUMN_MY_SIG_INTL:
        case LogbookModel::COLUMN_MY_SIG_INFO:
        case LogbookModel::COLUMN_MY_SIG_INFO_INTL:
        case LogbookModel::COLUMN_MY_SOTA_REF:
        case LogbookModel::COLUMN_MY_STATE:
        case LogbookModel::COLUMN_MY_STREET:
        case LogbookModel::COLUMN_MY_STREET_INTL:
        case LogbookModel::COLUMN_MY_USACA_COUNTIES:
        case LogbookModel::COLUMN_MY_VUCC_GRIDS:
            myInfoCheckboxList.append(columnCheckbox);
            break;

        case LogbookModel::COLUMN_QSL_RCVD:
        case LogbookModel::COLUMN_QSL_RCVD_DATE:
        case LogbookModel::COLUMN_QSL_SENT:
        case LogbookModel::COLUMN_QSL_SENT_DATE:
        case LogbookModel::COLUMN_EQSL_QSLRDATE:
        case LogbookModel::COLUMN_EQSL_QSLSDATE:
        case LogbookModel::COLUMN_EQSL_QSL_RCVD:
        case LogbookModel::COLUMN_EQSL_QSL_SENT:
        case LogbookModel::COLUMN_QSLMSG:
        case LogbookModel::COLUMN_QSLMSG_INTL:
        case LogbookModel::COLUMN_QSL_RCVD_VIA:
        case LogbookModel::COLUMN_QSL_SENT_VIA:
        case LogbookModel::COLUMN_QSL_VIA:
        case LogbookModel::COLUMN_LOTW_RCVD:
        case LogbookModel::COLUMN_LOTW_SENT:
        case LogbookModel::COLUMN_LOTW_RCVD_DATE:
        case LogbookModel::COLUMN_LOTW_SENT_DATE:
        case LogbookModel::COLUMN_QRZCOM_QSO_UPLOAD_DATE:
        case LogbookModel::COLUMN_QRZCOM_QSO_UPLOAD_STATUS:
        case LogbookModel::COLUMN_CLUBLOG_QSO_UPLOAD_DATE:
        case LogbookModel::COLUMN_CLUBLOG_QSO_UPLOAD_STATUS:
        case LogbookModel::COLUMN_HRDLOG_QSO_UPLOAD_DATE:
        case LogbookModel::COLUMN_HRDLOG_QSO_UPLOAD_STATUS:
            qslInfoCheckboxList.append(columnCheckbox);
            break;

        case LogbookModel::COLUMN_TEN_TEN:
        case LogbookModel::COLUMN_SIG:
        case LogbookModel::COLUMN_SIG_INTL:
        case LogbookModel::COLUMN_SIG_INFO:
        case LogbookModel::COLUMN_SIG_INFO_INTL:
        case LogbookModel::COLUMN_FISTS:
        case LogbookModel::COLUMN_FISTS_CC:
        case LogbookModel::COLUMN_SKCC:
        case LogbookModel::COLUMN_UKSMG:
        case LogbookModel::COLUMN_DARC_DOK:
            membersInfoCheckboxList.append(columnCheckbox);
            break;

        case LogbookModel::COLUMN_VUCC_GRIDS:
        case LogbookModel::COLUMN_VE_PROV:
        case LogbookModel::COLUMN_USACA_COUNTIES:
        case LogbookModel::COLUMN_PUBLIC_KEY:
        case LogbookModel::COLUMN_FIELDS:
        case LogbookModel::COLUMN_AWARD_GRANTED:
        case LogbookModel::COLUMN_AWARD_SUBMITTED:
        case LogbookModel::COLUMN_CREDIT_GRANTED:
        case LogbookModel::COLUMN_CREDIT_SUBMITTED:
        case LogbookModel::COLUMN_CLASS:
        case LogbookModel::COLUMN_AGE:
        case LogbookModel::COLUMN_COMMENT_INTL:
        case LogbookModel::COLUMN_COUNTRY_INTL:
        case LogbookModel::COLUMN_QTH_INTL:
        case LogbookModel::COLUMN_REGION:
        case LogbookModel::COLUMN_RIG_INTL:
        case LogbookModel::COLUMN_SILENT_KEY:
        case LogbookModel::COLUMN_WEB:
        case LogbookModel::COLUMN_ADDRESS_INTL:
        case LogbookModel::COLUMN_GUEST_OP:
        case LogbookModel::COLUMN_FORCE_INIT:
        case LogbookModel::COLUMN_MAX_BURSTS:
        case LogbookModel::COLUMN_MS_SHOWER:
        case LogbookModel::COLUMN_NR_PINGS:
        case LogbookModel::COLUMN_NR_BURSTS:
        case LogbookModel::COLUMN_QSO_RANDOM:
        case LogbookModel::COLUMN_QSO_COMPLETE:
        case LogbookModel::COLUMN_LAT:
        case LogbookModel::COLUMN_LON:
        case LogbookModel::COLUMN_OWNER_CALLSIGN:
        case LogbookModel::COLUMN_CONTACTED_OP:
        case LogbookModel::COLUMN_NOTES_INTL:
        case LogbookModel::COLUMN_STATION_CALLSIGN:
            otherInfoCheckboxList.append(columnCheckbox);
            break;

        case LogbookModel::COLUMN_A_INDEX:
        case LogbookModel::COLUMN_K_INDEX:
        case LogbookModel::COLUMN_PROP_MODE:
        case LogbookModel::COLUMN_SFI:
            conditionsCheckboxList.append(columnCheckbox);
            break;

        case LogbookModel::COLUMN_PRECEDENCE:
        case LogbookModel::COLUMN_CONTEST_ID:
        case LogbookModel::COLUMN_SRX:
        case LogbookModel::COLUMN_SRX_STRING:
        case LogbookModel::COLUMN_STX:
        case LogbookModel::COLUMN_STX_STRING:
        case LogbookModel::COLUMN_CHECK:
            contestCheckboxList.append(columnCheckbox);
            break;

        default:
            generalCheckboxList.append(columnCheckbox);
        }

        columnIndex++;
    }

    addSortedCheckboxes(ui->generalInfoGrid, generalCheckboxList, checkboxesPerRow);
    addSortedCheckboxes(ui->myInfoGrid, myInfoCheckboxList, checkboxesPerRow);
    addSortedCheckboxes(ui->qslInfoGrid, qslInfoCheckboxList, checkboxesPerRow);
    addSortedCheckboxes(ui->membersInfoGrig, membersInfoCheckboxList, checkboxesPerRow);
    addSortedCheckboxes(ui->conditionsInfoGrid, conditionsCheckboxList, checkboxesPerRow);
    addSortedCheckboxes(ui->contestsInfoGrid, contestCheckboxList, checkboxesPerRow);
    addSortedCheckboxes(ui->otherInfoGrid, otherInfoCheckboxList, checkboxesPerRow);

    addSelectUnselect(ui->generalInfoGrid, checkboxesPerRow);
    addSelectUnselect(ui->myInfoGrid, checkboxesPerRow);
    addSelectUnselect(ui->qslInfoGrid, checkboxesPerRow);
    addSelectUnselect(ui->membersInfoGrig, checkboxesPerRow);
    addSelectUnselect(ui->conditionsInfoGrid, checkboxesPerRow);
    addSelectUnselect(ui->contestsInfoGrid, checkboxesPerRow);
    addSelectUnselect(ui->otherInfoGrid, checkboxesPerRow);
}

void ColumnSettingDialog::addSortedCheckboxes(QGridLayout *grid, QList<QCheckBox*> &checkboxlist, int elementsPerRow)
{
    FCT_IDENTIFICATION;

    int elementIndex = 0;

    std::sort(
            checkboxlist.begin(),
            checkboxlist.end(),
            [](const QCheckBox* a, const QCheckBox *b)->bool
    {
       return a->text() < b->text();
    });

    for (auto item: checkboxlist)
    {
        grid->addWidget(item, elementIndex/elementsPerRow, elementIndex%elementsPerRow);
        elementIndex++;
    }
}
void ColumnSettingDialog::addSelectUnselect(QGridLayout *grid, int elementsPerRow)
{
    FCT_IDENTIFICATION;

    const QString unselectAllLabel = tr("Unselect All");
    const QString selectAllLabel = tr("Select All");

    QPushButton *buttonUnselectAll=new QPushButton();
    QPushButton *buttonSelectAll=new QPushButton();

    buttonUnselectAll->setText(unselectAllLabel);
    buttonSelectAll->setText(selectAllLabel);

    int current_rows = grid->rowCount();

    grid->addWidget(buttonUnselectAll, current_rows + 1, 0);
    grid->addWidget(buttonSelectAll, current_rows + 1, elementsPerRow - 1);

    connect(buttonUnselectAll, &QPushButton::clicked, this, [grid]() {
        for(int idx = 0; idx < grid->count(); idx++)
        {
            QLayoutItem *item = grid->itemAt(idx);
            if ( !item || !item->widget() )
            {
                continue;
            }
            QCheckBox *checkbox = qobject_cast<QCheckBox*>(item->widget());
            if ( checkbox )
            {
                checkbox->setChecked(false);
            }
        }
    });

    connect(buttonSelectAll, &QPushButton::clicked, this, [grid]() {
        for(int idx = 0; idx < grid->count(); idx++)
        {
            QLayoutItem *item = grid->itemAt(idx);
            if ( !item || !item->widget() )
            {
                continue;
            }
            QCheckBox *checkbox = qobject_cast<QCheckBox*>(item->widget());
            if ( checkbox )
            {
                checkbox->setChecked(true);
            }
        }
    });
}

ColumnSettingDialog::~ColumnSettingDialog()
{
    FCT_IDENTIFICATION;
    delete ui;
}
