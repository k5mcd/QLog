#include "AwardsTableModel.h"
#include <QColor>

AwardsTableModel::AwardsTableModel(QObject* parent) :
    QSqlQueryModel(parent)
{

}


QVariant AwardsTableModel::data(const QModelIndex &index, int role) const
{

    if ( index.column() >= 3
         /* using hiddne column 0 to identify type of information */
         && this->data(this->index(index.row(),0), Qt::DisplayRole).toInt() >= 3
         && role == Qt::BackgroundRole )
    {
        if (this->data(index, Qt::DisplayRole).toInt() > 1 )
        {
            return QColor(Qt::green);
        }
        else if ( this->data(index, Qt::DisplayRole).toInt() == 1 )
        {
            return QColor(Qt::yellow);
        }
    }
    else if ( index.column() >= 3
              /* using hiddne column 0 to identify type of information */
              && this->data(this->index(index.row(),0), Qt::DisplayRole).toInt() >= 3
              && role == Qt::ToolTipRole )
    {
        if (this->data(index, Qt::DisplayRole).toInt() > 1 )
        {

            return QString(tr("Confirmed"));
        }
        else if ( this->data(index, Qt::DisplayRole).toInt() == 1 )
        {
            return QString(tr("Worked"));
        }
        else
        {
            return QString(tr("Still Waiting"));
        }
    }
    else if ( index.column() >=  3
              && this->data(this->index(index.row(),0), Qt::DisplayRole).toInt() >= 3
              && role == Qt::TextColorRole)
    {
        //return this->data(index,Qt::BackgroundRole);
        return QColor(Qt::transparent);
    }
    return QSqlQueryModel::data(index, role);
}
