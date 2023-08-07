#ifndef EDITLINE_H
#define EDITLINE_H

#include <QObject>
#include <QLineEdit>

class NewContactEditLine : public QLineEdit
{
    Q_OBJECT

public:
    explicit NewContactEditLine(QWidget *parent = nullptr);
    void setText(const QString & text);

protected:
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
};

#endif // EDITLINE_H
