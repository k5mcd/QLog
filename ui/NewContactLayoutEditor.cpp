#include <QMessageBox>

#include "NewContactLayoutEditor.h"
#include "ui_NewContactLayoutEditor.h"
#include "core/debug.h"
#include "data/NewContactLayoutProfile.h"
#include "ui/NewContactWidget.h"

MODULE_IDENTIFICATION("qlog.ui.NewContactLayoutEditor");

NewContactLayoutEditor::NewContactLayoutEditor(const QString &layoutName,
                                               QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewContactLayoutEditor),
    availableFieldsModel(new StringListModel(this)),
    rowAFieldsModel(new StringListModel(this)),
    rowBFieldsModel(new StringListModel(this))
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    logbookmodel = new LogbookModel(this);

    for ( int fieldIndex : NewContactWidget::customizableFields )
    {
        QString fieldName = logbookmodel->headerData(fieldIndex, Qt::Horizontal).toString();
        fieldIndex2Name[fieldIndex] = fieldName;
        availableFieldsModel->append(fieldName);
    }

    availableFieldsModel->sort(0);

    ui->availableFieldsListView->setModel(availableFieldsModel);
    ui->rowAFieldsListView->setModel(rowAFieldsModel);
    ui->rowBFieldsListView->setModel(rowBFieldsModel);

    if ( ! layoutName.isEmpty() )
    {
        loadLayout(layoutName);
    }
}

NewContactLayoutEditor::~NewContactLayoutEditor()
{
    delete ui;
}

void NewContactLayoutEditor::save()
{
    FCT_IDENTIFICATION;

    if ( ui->profileNameEdit->text().isEmpty() )
    {
        ui->profileNameEdit->setPlaceholderText(tr("Must not be empty"));
        return;
    }

    if ( layoutNameExists(ui->profileNameEdit->text()) )
    {
        QMessageBox::warning(nullptr, QMessageBox::tr("QLog Info"),
                              QMessageBox::tr("Layout name is already exists."));
        return;
    }

    NewContactLayoutProfile profile;

    profile.profileName = ui->profileNameEdit->text();
    profile.rowA = getFieldIndexes(rowAFieldsModel);
    profile.rowB = getFieldIndexes(rowBFieldsModel);
    NewContactLayoutProfilesManager::instance()->addProfile(profile.profileName, profile);
    NewContactLayoutProfilesManager::instance()->save();

    accept();
}

void NewContactLayoutEditor::moveToRowAButton()
{
    FCT_IDENTIFICATION;

    moveField(availableFieldsModel,
              rowAFieldsModel,
              ui->availableFieldsListView->selectionModel()->selectedIndexes());
}

void NewContactLayoutEditor::moveToRowBButton()
{
    FCT_IDENTIFICATION;

    moveField(availableFieldsModel,
              rowBFieldsModel,
              ui->availableFieldsListView->selectionModel()->selectedRows());
}

void NewContactLayoutEditor::removeFromRowAButton()
{
    FCT_IDENTIFICATION;

    moveField(rowAFieldsModel,
              availableFieldsModel,
              ui->rowAFieldsListView->selectionModel()->selectedRows());
}

void NewContactLayoutEditor::removeFromRowBButton()
{
    FCT_IDENTIFICATION;

    moveField(rowBFieldsModel,
              availableFieldsModel,
              ui->rowBFieldsListView->selectionModel()->selectedRows());
}

void NewContactLayoutEditor::rowAUpButton()
{
    FCT_IDENTIFICATION;

    QModelIndexList modelList = ui->rowAFieldsListView->selectionModel()->selectedRows();

    if ( modelList.size() > 0 )
    {
        rowAFieldsModel->moveUp(modelList.at(0));
    }
}

void NewContactLayoutEditor::rowBUpButton()
{
    FCT_IDENTIFICATION;

    QModelIndexList modelList = ui->rowBFieldsListView->selectionModel()->selectedRows();

    if ( modelList.size() > 0 )
    {
        rowBFieldsModel->moveUp(modelList.at(0));
    }
}

void NewContactLayoutEditor::rowADownButton()
{
    FCT_IDENTIFICATION;

    QModelIndexList modelList = ui->rowAFieldsListView->selectionModel()->selectedRows();

    if ( modelList.size() > 0 )
    {
        rowAFieldsModel->moveDown(modelList.at(0));
    }
}

void NewContactLayoutEditor::rowBDownButton()
{
    FCT_IDENTIFICATION;
    QModelIndexList modelList = ui->rowBFieldsListView->selectionModel()->selectedRows();

    if ( modelList.size() > 0 )
    {
        rowBFieldsModel->moveDown(modelList.at(0));
    }
}

void NewContactLayoutEditor::profileNameChanged(const QString &profileName)
{
    FCT_IDENTIFICATION;

    QPalette p;

    if ( layoutNameExists(profileName) )
    {
        p.setColor(QPalette::Text,Qt::red);
    }
    else
    {
        p.setColor(QPalette::Text,qApp->palette().text().color());
    }

    ui->profileNameEdit->setPalette(p);
}

void NewContactLayoutEditor::moveField(StringListModel *source,
                                       StringListModel *destination,
                                       const QModelIndexList &sourceIndexList)
{
    FCT_IDENTIFICATION;

    QModelIndexList selectedIndexes = sourceIndexList;

    if ( selectedIndexes.isEmpty() )
            return;

    std::sort(selectedIndexes.begin(),
              selectedIndexes.end(),
              [](const QModelIndex &a, const QModelIndex &b)
    {
        return a.row() > b.row();
    });

    for ( const QModelIndex &index : sourceIndexList )
    {
        destination->append(source->data(index).toString());
    }

    /* Delete the sorted index list becuase without sorting
     * it deletes wrong records
     */
    for (const QModelIndex &index : selectedIndexes)
    {
        source->deleteItem(index);
    }
}

QList<int> NewContactLayoutEditor::getFieldIndexes(StringListModel *model)
{
    FCT_IDENTIFICATION;

    QStringList list = model->stringList();
    QList<int> ret;

    for ( const QString &fieldName : qAsConst(list) )
    {
        ret << fieldIndex2Name.key(fieldName);
    }

    return ret;
}

void NewContactLayoutEditor::loadLayout(const QString &layoutName)
{
    FCT_IDENTIFICATION;

    NewContactLayoutProfile profile = NewContactLayoutProfilesManager::instance()->getProfile(layoutName);

    ui->profileNameEdit->setEnabled(false);
    ui->profileNameEdit->setText(profile.profileName);

    for ( int fieldIndex : qAsConst(profile.rowA) )
    {
        QString fieldName = fieldIndex2Name.value(fieldIndex);
        rowAFieldsModel->append(fieldName);
        availableFieldsModel->deleteItem(fieldName);
    }

    for ( int fieldIndex : qAsConst(profile.rowB) )
    {
        QString fieldName = fieldIndex2Name.value(fieldIndex);
        rowBFieldsModel->append(fieldName);
        availableFieldsModel->deleteItem(fieldName);
    }
}

bool NewContactLayoutEditor::layoutNameExists(const QString &layoutName)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << layoutName;

    return  ui->profileNameEdit->isEnabled()
            && NewContactLayoutProfilesManager::instance()->profileNameList().contains(layoutName);
}
