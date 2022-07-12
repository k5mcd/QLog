#ifndef QTABLEQSOVIEW_H
#define QTABLEQSOVIEW_H

#include <QTableView>
#include <QObject>

class QTableQSOView : public QTableView
{
    Q_OBJECT

signals:
    void dataCommitted();

public:
    explicit QTableQSOView(QWidget *parent = nullptr);
    void commitData(QWidget *editor) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // QTABLEQSOVIEW_H
