#ifndef DXCCTABLEWIDGET_H
#define DXCCTABLEWIDGET_H

#include <QWidget>
#include <QTableView>
#include "data/Band.h"

class DxccTableModel;

class DxccTableWidget : public QTableView
{
    Q_OBJECT
public:
    explicit DxccTableWidget(QWidget *parent = nullptr);

signals:

public slots:
    void clear();
    void setDxcc(int dxcc, Band band);

private:
    DxccTableModel* dxccTableModel;
    QStringList headerStrings;
};

#endif // DXCCTABLEWIDGET_H
