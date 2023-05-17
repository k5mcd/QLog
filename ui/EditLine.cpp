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
    home(false);
}
