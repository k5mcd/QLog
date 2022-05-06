#ifndef ALERTRULEDETAIL_H
#define ALERTRULEDETAIL_H

#include <QDialog>

namespace Ui {
class AlertRuleDetail;
}

class AlertRuleDetail : public QDialog
{
    Q_OBJECT

public:
    explicit AlertRuleDetail(const QString &ruleName, QWidget *parent);
    ~AlertRuleDetail();

public slots:
    void save();
    void ruleNameChanged(const QString&);
    void callsignChanged(const QString&);
    void spotCommentChanged(const QString&);

private:
    Ui::AlertRuleDetail *ui;
    QString ruleName;
    QStringList ruleNamesList;


private:
    bool ruleExists(const QString &ruleName);
    void loadRule(const QString &ruleName);
};

#endif // ALERTRULEDETAIL_H
