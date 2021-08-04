#ifndef STYLEITEMDELEGATE_H
#define STYLEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QDate>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QStyleOptionViewItem>
#include <QPainter>

class CallsignDelegate : public QStyledItemDelegate {
public:
    CallsignDelegate(QObject* parent = 0) :
        QStyledItemDelegate(parent) { }

    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
        QStyledItemDelegate::initStyleOption(option, index);
        option->font.setBold(true);
    }
};

class DateFormatDelegate : public QStyledItemDelegate {
public:
    DateFormatDelegate(QObject* parent = 0) :
        QStyledItemDelegate(parent) { }

    QString displayText(const QVariant& value, const QLocale& locale) const {
        return value.toDate().toString(locale.dateFormat(QLocale::ShortFormat));
    }
};

class TimeFormatDelegate : public QStyledItemDelegate {
public:
    TimeFormatDelegate(QObject* parent = 0) :
        QStyledItemDelegate(parent) { }

    QString displayText(const QVariant& value, const QLocale& locale) const {
        return value.toTime().toString(locale.timeFormat(QLocale::ShortFormat));
    }
};

class TimestampFormatDelegate : public QStyledItemDelegate {
public:
    TimestampFormatDelegate(QObject* parent = 0) :
        QStyledItemDelegate(parent) { }

    QString displayText(const QVariant& value, const QLocale& locale) const {
        return value.toDateTime().toTimeSpec(Qt::UTC).toString(locale.dateTimeFormat(QLocale::ShortFormat));
    }
};

class UnitFormatDelegate : public QStyledItemDelegate {
public:
    UnitFormatDelegate(QString unit, int precision, double step, QObject* parent = 0) :
        QStyledItemDelegate(parent), unit(unit), precision(precision), step(step) { }

    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const {
        QStyledItemDelegate::initStyleOption(option, index);
        option->displayAlignment = Qt::AlignVCenter | Qt::AlignRight;
    }

    QString displayText(const QVariant& value, const QLocale&) const {
        return QString("%1 %2").arg(QString::number(value.toDouble(), 'f', 3), unit);
    }

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem&,
                          const QModelIndex&) const
    {
        QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
        editor->setDecimals(precision);
        editor->setRange(0, 1e12);
        editor->setSingleStep(step);
        return editor;
    }

    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex&) const
    {
        editor->setGeometry(option.rect);
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const
    {
        double value = index.model()->data(index, Qt::EditRole).toDouble();
        QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->setValue(value);
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const
    {
        QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->interpretText();
        double value = spinBox->value();
        model->setData(index, value, Qt::EditRole);
    }

private:
    QString unit;
    int precision;
    double step;
};

class CheckBoxDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    CheckBoxDelegate(QObject *parent = 0 ) :QItemDelegate(parent){};

    void paint( QPainter *painter,
                const QStyleOptionViewItem &option,
                const QModelIndex &index ) const
    {
        bool is_enabled = index.model()->data(index, Qt::DisplayRole).toBool();
        if ( !is_enabled) painter->fillRect(option.rect, option.palette.dark());
        drawDisplay(painter,option,option.rect,is_enabled? QString("     ").append(tr("Enabled"))
                                                         : QString("     ").append(tr("Disabled")));
        drawFocus(painter,option,option.rect);

    };


    QWidget *createEditor( QWidget *parent,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index ) const
    {
        (void)option;
        (void)index;

        theCheckBox = new QCheckBox( parent );
        QObject::connect(theCheckBox,SIGNAL(toggled(bool)),this,SLOT(setData(bool)));
        return theCheckBox;
    };

    void setEditorData( QWidget *editor,
                        const QModelIndex &index ) const
    {
        bool val = index.model()->data( index, Qt::DisplayRole ).toBool();

        (static_cast<QCheckBox*>( editor ))->setChecked(val);

    }

    void setModelData( QWidget *editor,
                        QAbstractItemModel *model,
                        const QModelIndex &index ) const
    {
        model->setData( index, (bool)(static_cast<QCheckBox*>( editor )->isChecked() ) );
    }


    void updateEditorGeometry( QWidget *editor,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index ) const
    {
        (void)index;
        editor->setGeometry( option.rect );
    }

    mutable QCheckBox * theCheckBox;

private slots:

    void setData(bool val)
    {
        (void)val;
        emit commitData(theCheckBox);
    }
};

#endif // STYLEITEMDELEGATE_H
