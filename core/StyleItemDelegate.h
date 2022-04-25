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
#include <QDebug>
#include <QTextEdit>

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

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem&,
                          const QModelIndex&) const
    {
        QDateEdit* editor = new QDateEdit(parent);
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
        QDate value = index.model()->data(index, Qt::EditRole).toDate();
        QDateEdit* timeEdit = static_cast<QDateEdit*>(editor);
        timeEdit->setDate(value);
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const
    {
        QDateEdit* dateEdit = static_cast<QDateEdit*>(editor);
        dateEdit->interpretText();
        QDateTime value = dateEdit->dateTime();
        model->setData(index, value, Qt::EditRole);
    }
};

class TimeFormatDelegate : public QStyledItemDelegate {
public:
    TimeFormatDelegate(QObject* parent = 0) :
        QStyledItemDelegate(parent) { }

    QString displayText(const QVariant& value, const QLocale& locale) const {
        return value.toTime().toString(locale.timeFormat(QLocale::LongFormat)).remove("UTC"); //hack: remove timezone from LongFormat
    }
};

class TimestampFormatDelegate : public QStyledItemDelegate {
public:
    TimestampFormatDelegate(QObject* parent = 0) :
        QStyledItemDelegate(parent) { }

    QString displayText(const QVariant& value, const QLocale& locale) const {
        //return value.toDateTime().toTimeSpec(Qt::UTC).toString(locale.dateTimeFormat(QLocale::ShortFormat));
        return value.toDateTime().toTimeSpec(Qt::UTC).toString(locale.dateFormat(QLocale::ShortFormat) + " " + locale.timeFormat(QLocale::LongFormat)).remove("UTC"); //hack: remove timezone from LongFormat
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
        option->displayAlignment = Qt::AlignVCenter | Qt::AlignLeft;
    }

    QString displayText(const QVariant& value, const QLocale&) const {
        return QString("%1 %2").arg(QString::number(value.toDouble(), 'f', precision), unit);
    }

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem&,
                          const QModelIndex&) const
    {
        QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
        editor->setDecimals(precision);
        editor->setRange(-1*step, 1e12);
        editor->setSingleStep(step);
        editor->setSpecialValueText("Empty");
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
        if (spinBox->text() == "Empty" )
        {
            model->setData(index, QVariant() , Qt::EditRole);
            return;
        }
        spinBox->interpretText();
        double value = spinBox->value();
        model->setData(index,value, Qt::EditRole);
    }

private:
    QString unit;
    int precision;
    double step;
};


class ComboFormatDelegate : public QStyledItemDelegate {
public:
    ComboFormatDelegate(QAbstractTableModel* in_model, QObject* parent = nullptr):
        QStyledItemDelegate(parent)
    {
        model = in_model;
    }
    ComboFormatDelegate(QStringList in_list, QObject* parent = nullptr):
        QStyledItemDelegate(parent)
    {
        model = nullptr;
        list = in_list;
    }

    ComboFormatDelegate(QMap<QString, QString> in_map, QObject* parent = nullptr):
        QStyledItemDelegate(parent)
    {
        model = nullptr;
        map = in_map;
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
        combo->clear();

        if ( model )
        {
            combo->setModel(model);
            combo->setCurrentText(index.model()->data(index).toString());
        }
        else if ( ! list.isEmpty() )
        {
            combo->addItems(list);
            combo->setCurrentText(index.model()->data(index).toString());
        }
        else if ( ! map.isEmpty() )
        {
            QMapIterator<QString, QString> iter(map);
            int iter_index = 0;
            int value_index = 0;

            while ( iter.hasNext() )
            {
                iter.next();
                combo->addItem(iter.value(), iter.key());
                if ( iter.key() == index.model()->data(index).toString() )
                {
                    value_index = iter_index;
                }
                iter_index++;
            }
            combo->setCurrentIndex(value_index);
        }
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const
    {
       QComboBox* combo = static_cast<QComboBox*>(editor);
       QString curr_value;

       if ( !map.isEmpty() )
       {
           curr_value = combo->currentData().toString();
       }
       else
       {
           curr_value = combo->currentText();
       }

       if ( curr_value == " " )
       {
           model->setData(index, QVariant(), Qt::EditRole);
       }
       else
       {
           model->setData(index, QVariant(curr_value), Qt::EditRole);
       }
    }

private:
    QAbstractTableModel *model;
    QStringList list;
    QMap<QString, QString> map;
};



class CheckBoxDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    CheckBoxDelegate(QObject *parent = nullptr ) : QItemDelegate(parent), theCheckBox(nullptr) {};

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

class TextBoxDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    TextBoxDelegate(QObject *parent = 0 ) :QItemDelegate(parent), theText(nullptr){};

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem&,
                          const QModelIndex&) const
    {
        theText = new QTextEdit(parent);

        return theText;
    }

    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex&) const
    {
        editor->setGeometry(option.rect.x(),option.rect.y(),editor->sizeHint().width(),editor->sizeHint().height());
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const
    {
        QString value = index.model()->data(index, Qt::EditRole).toString();
        QTextEdit* textEditor = static_cast<QTextEdit*>(editor);
        textEditor->setPlainText(value);
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const
    {
        QTextEdit* textEditor = static_cast<QTextEdit*>(editor);
        QString value = textEditor->toPlainText();
        model->setData(index,value, Qt::EditRole);
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QString line = index.model()->data(index, Qt::EditRole).toString().simplified();

        drawDisplay(painter,option,option.rect, line);
        drawFocus(painter, option, option.rect);
    }

    mutable QTextEdit *theText;

private slots:

    void setData(bool val)
    {
        (void)val;
        emit commitData(theText);
    }
};


#endif // STYLEITEMDELEGATE_H
