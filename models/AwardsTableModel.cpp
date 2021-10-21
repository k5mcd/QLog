#include "AwardsTableModel.h"
#include <QColor>
#include <QFont>

AwardsTableModel::AwardsTableModel(QObject* parent) :
    QSqlQueryModel(parent)
{

}


QVariant AwardsTableModel::data(const QModelIndex &index, int role) const
{
    /* using hiden column 0 to identify type of information */

    if ( index.column() >= 3
         && role == Qt::BackgroundRole
         /* using hiddne column 0 to identify type of information */
         &&  this->data(this->index(index.row(),0), Qt::DisplayRole).toInt() >= 3 )
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
    else if ( role == Qt::FontRole
             && ( this->data(this->index(index.row(),0), Qt::DisplayRole).toInt() == 0
                  || this->data(this->index(index.row(),0), Qt::DisplayRole).toInt() == 1
                  || this->data(this->index(index.row(),0), Qt::DisplayRole).toInt() == 2 ) )
    {
        QFont font;
        font.setBold(true);
        return font;
    }
    else if ( index.column() >= 3
              && role == Qt::ToolTipRole
              && this->data(this->index(index.row(),0), Qt::DisplayRole).toInt() >= 3 )
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
              && role == Qt::TextColorRole
              && this->data(this->index(index.row(),0), Qt::DisplayRole).toInt() >= 3 )
    {
        //return this->data(index,Qt::BackgroundRole);
        return QColor(Qt::transparent);
    }

    return QSqlQueryModel::data(index, role);
}
