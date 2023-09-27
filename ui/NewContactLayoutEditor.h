#ifndef NEWCONTACTLAYOUTEDITOR_H
#define NEWCONTACTLAYOUTEDITOR_H

#include <QDialog>
#include <QPointer>
#include <QStringListModel>
#include "models/LogbookModel.h"
#include "core/debug.h"
#include "ui/NewContactWidget.h"
#include "data/NewContactLayoutProfile.h"

namespace Ui {
class NewContactLayoutEditor;
}

class StringListModel : public QStringListModel
{
public:
    StringListModel(QObject *parent = nullptr) :
        QStringListModel(parent){};

    void append (const QString& string)
    {
        insertRows(rowCount(), 1);
        setData(index(rowCount()-1), string);
    };

    void deleteItem(const QModelIndex &index)
    {
        if (!index.isValid() || index.row() >= stringList().size())
                return;
        removeRow(index.row());
    }

    void deleteItem(const QString &value)
    {
        QModelIndexList itemIndexList = match(index(0,0),
              Qt::DisplayRole,
              value,
              1,
              Qt::MatchExactly);

        for (const QModelIndex & itemIndex: qAsConst(itemIndexList))
        {
            deleteItem(itemIndex);
        }
    }

    void moveUp(const QModelIndex &index)
    {
        if ( index.row() - 1 < 0 )
            return;

        moveRow(index.parent(), index.row(), index.parent(), index.row() - 1);
    }

    void moveDown(const QModelIndex &inIndex)
    {
        if ( inIndex.row() + 1 >= rowCount() )
            return;

         moveUp(index(inIndex.row()+1));
    }

    StringListModel& operator<<(const QString& string)
    {
        append(string);
        return *this;
    };
};

class NewContactLayoutEditor : public QDialog
{
    Q_OBJECT

public:
    explicit NewContactLayoutEditor(const QString &layoutName = QString(),
                                    QWidget *parent = nullptr);
    ~NewContactLayoutEditor();

private:
    Ui::NewContactLayoutEditor *ui;
    StringListModel *availableFieldsModel;
    StringListModel *qsoRowAFieldsModel;
    StringListModel *qsoRowBFieldsModel;
    StringListModel *detailColAFieldsModel;
    StringListModel *detailColBFieldsModel;
    StringListModel *detailColCFieldsModel;

    NewContactDynamicWidgets *dynamicWidgets;

private slots:
    void save();
    void profileNameChanged(const QString&);

private:
    void fillWidgets(const NewContactLayoutProfile &profile);
    bool layoutNameExists(const QString &layoutName);
    void moveField(StringListModel *source,
                   StringListModel *destination,
                   const QModelIndexList &sourceIndexList);
    void connectQSORowButtons();
    void connectDetailColsButtons();
    QList<int> getFieldIndexes(StringListModel *model);
};

#endif // NEWCONTACTLAYOUTEDITOR_H
