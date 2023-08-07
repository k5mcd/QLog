#include <QFocusEvent>
#include "EditLine.h"

NewContactEditLine::NewContactEditLine(QWidget *parent) :
    QLineEdit(parent)
{

}

void NewContactEditLine::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);

//   Deselect text when focus - maybe later
//    if ( hasSelectedText() )
//    {
//        deselect();
//    }

}

void NewContactEditLine::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);

    Qt::FocusReason reason = event->reason();

    if ( reason != Qt::ActiveWindowFocusReason &&
         reason != Qt::PopupFocusReason )
    {
        home(false);
    }
}

void NewContactEditLine::setText(const QString &text)
{
    QLineEdit::setText(text);
    home(false);
}
