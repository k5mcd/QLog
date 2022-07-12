#include <QAbstractItemModel>
#include <QKeyEvent>
#include "QTableQSOView.h"
#include "models/LogbookModel.h"

QTableQSOView::QTableQSOView(QWidget *parent) :
    QTableView(parent)
{ }

void QTableQSOView::commitData(QWidget *editor)
{
    QTableView::commitData(editor);

    QAbstractItemModel *model = this->model();
    QVariant value = model->data(this->currentIndex(), Qt::EditRole);
    int currRow = this->currentIndex().row();
    int currCol = this->currentIndex().column();

    /* Group Editing Support */
    /* If rows are selected then update them*/
    foreach (auto index, this->selectionModel()->selectedRows())
    {
        if ( index.row() != currRow // Do not update the same row again
             /* Protect selected columns against group editing */
             && index.column() != LogbookModel::COLUMN_CALL
             && index.column() != LogbookModel::COLUMN_TIME_ON
             && index.column() != LogbookModel::COLUMN_TIME_OFF )
        {
            model->setData(model->index(index.row(),currCol), value, Qt::EditRole);
        }
    }

    emit dataCommitted();
}

void QTableQSOView::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_F2 )
    {
        return;
    }

    return QTableView::keyPressEvent(event);
};


