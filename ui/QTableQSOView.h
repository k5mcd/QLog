#ifndef QLOG_UI_QTABLEQSOVIEW_H
#define QLOG_UI_QTABLEQSOVIEW_H

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

#endif // QLOG_UI_QTABLEQSOVIEW_H
