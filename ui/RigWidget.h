#ifndef RIGWIDGET_H
#define RIGWIDGET_H

#include <QWidget>

namespace Ui {
class RigWidget;
}

class RigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RigWidget(QWidget *parent = nullptr);
    ~RigWidget();

signals:
    void rigProfileChanged();

public slots:
    void updateFrequency(double freq);
    void updateMode(QString mode);
    void updatePWR(double);
    void updateVFO(unsigned int);
    void bandComboChanged(QString);
    void rigProfileComboChanged(QString);
    void refreshRigProfileCombo();
    void refreshBandCombo();
    void reloadSettings();
    void rigConnected();
    void rigDisconnected();


private:

    void resetRigInfo();

    Ui::RigWidget *ui;
};

#endif // RIGWIDGET_H
