#include <QFocusEvent>
#include "EditLine.h"

NewContactEditLine::NewContactEditLine(QWidget *parent) :
    QLineEdit(parent),
    spaceForbiddenFlag(false)
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

void NewContactEditLine::keyPressEvent(QKeyEvent *event)
{
    if ( spaceForbiddenFlag && event->key() == Qt::Key_Space )
        focusNextChild();
    else
        QLineEdit::keyPressEvent(event);
}

void NewContactEditLine::setText(const QString &text)
{
    QLineEdit::setText(text);
    home(false);
}

void NewContactEditLine::spaceForbidden(bool inSpaceForbidden)
{
    spaceForbiddenFlag = inSpaceForbidden;
}
