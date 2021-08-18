#ifndef STYLEITEMDELEGATE_H
#define STYLEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QDate>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QAbstractItemModel>

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

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem&,
                          const QModelIndex&) const
    {
        QDateTimeEdit* editor = new QDateTimeEdit(parent);
        editor->setTimeSpec(Qt::UTC);
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
        QDateTime value = index.model()->data(index, Qt::EditRole).toDateTime();
        QDateTimeEdit* timeEdit = static_cast<QDateTimeEdit*>(editor);
        timeEdit->setDateTime(value);
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const
    {
        QDateTimeEdit* timeEdit = static_cast<QDateTimeEdit*>(editor);
        timeEdit->interpretText();
        QDateTime value = timeEdit->dateTime();
        model->setData(index, value, Qt::EditRole);
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


class ComboFormatDelegate : public QStyledItemDelegate {
public:
    ComboFormatDelegate(QAbstractTableModel* in_model, QObject* parent = 0)
    {
        model = in_model;
    }
    ComboFormatDelegate(QStringList in_list, QObject* parent = 0)
    {
        model = nullptr;
        list = in_list;
    }

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem&,
                          const QModelIndex&) const
    {
        QComboBox *editor = new QComboBox(parent);
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
        QComboBox* combo = static_cast<QComboBox*>(editor);
        if ( model )
        {
            combo->setModel(model);
            combo->setCurrentText(index.model()->data(index).toString());
        }
        else
        {
            combo->addItems(list);
            combo->setCurrentText(index.model()->data(index).toString());
        }

    }

    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const
    {
       QComboBox* combo = static_cast<QComboBox*>(editor);
       model->setData(index, QVariant(combo->currentText()), Qt::EditRole);
    }

private:
    QAbstractTableModel *model;
    QStringList list;
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
