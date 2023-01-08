#include <QColor>
#include <QSize>
#include <QFont>
#include <QDebug>
#include <QBrush>
#include "DxccTableModel.h"
#include "data/Data.h"

DxccTableModel::DxccTableModel(QObject* parent) : QSqlQueryModel(parent) {}


QVariant DxccTableModel::data(const QModelIndex &index, int role) const
{
    if ( index.column() != 0 && role == Qt::TextAlignmentRole )
    {
        return int(Qt::AlignCenter | Qt::AlignVCenter);
    }
    else if ( index.column() != 0 && role == Qt::BackgroundRole )
    {
        QString currData = data(index, Qt::DisplayRole).toString();

        if ( currData.contains("L")
             || currData.contains("P"))
        {
            return Data::statusToColor(DxccStatus::NewMode, Qt::green);
        }

        if ( currData == QString("e")
             || currData == QString("W") )
        {
            return QColor(QColor(255,165,0));
        }

    }
    else if ( index.column() != 0 && role == Qt::DisplayRole )
    {
        QString currData = QSqlQueryModel::data(index, Qt::DisplayRole).toString();

        if ( currData.isEmpty() )
            return QString();

        if ( currData == QString("111") )
        {
            return QString("W");
        }

        QString ret;

        if ( currData[0] == "2" )
        {
            ret.append("e");
        }

        if ( currData[1] == "2" )
        {
            ret.append("L");
        }

        if ( currData[2] == "2" )
        {
            ret.append("P");
        }

        return QString(ret);
    }
    else if ( index.column() != 0 && role == Qt::ToolTipRole )
    {
        QString currData = data(index, Qt::DisplayRole).toString();
        QStringList ret;

        if ( currData.contains("W") )
        {
            ret.append(tr("Worked"));
        }
        if ( currData.contains("e") )
        {
            ret.append(tr("eQSL"));
        }
        if ( currData.contains("L") )
        {
            ret.append(tr("LoTW"));
        }
        if ( currData.contains("P") )
        {
            ret.append(tr("Paper"));
        }

        return ret.join(", ");
    }

    return QSqlQueryModel::data(index, role);
}
