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
    /* 0 - Total Worked/Confirmed row
     * 1 - Confirmed row
     * 2 - Worked row
     * 3 - Per DXCC Entity row
     */

    QVariant originRowType = QSqlQueryModel::data(this->index(index.row(),0), Qt::DisplayRole);
    QVariant originCellValue = QSqlQueryModel::data(index, Qt::DisplayRole);

    if ( index.column() >= 3
         && role == Qt::BackgroundRole
         &&  originRowType.toInt() >= 3 )
    {
        if ( originCellValue.toInt() > 1 )
        {
            return QColor(Qt::green);
        }
        else if ( originCellValue.toInt() == 1 )
        {
            return QColor(QColor(255,165,0));
        }
    }
    else if ( role == Qt::FontRole
             &&  originRowType.toInt() <= 2 )
    {
        QFont font;
        font.setBold(true);
        return font;
    }
    else if ( index.column() >= 3
              && role == Qt::ToolTipRole
              && originRowType.toInt() >= 3 )
    {
        if ( originCellValue.toInt() > 1 )
        {
            return QString(tr("Confirmed"));
        }
        else if ( originCellValue.toInt() == 1 )
        {
            return QString(tr("Worked"));
        }
        else
        {
            return QString(tr("Still Waiting"));
        }
    }
    else if ( index.column() >= 3
              && role == Qt::DisplayRole )
    {
        if ( originRowType.toInt() >= 3 )
        {
            return QString();
        }
        else
        {
            return originCellValue.toString();
        }
    }
    else if ( index.column() >=  3
              && role == Qt::ForegroundRole
              && originRowType.toInt() >= 3 )
    {
        return QColor(Qt::transparent);
    }

    return QSqlQueryModel::data(index, role);
}
