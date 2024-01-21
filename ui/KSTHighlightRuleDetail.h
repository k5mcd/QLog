#ifndef QLOG_UI_KSTHIGHLIGHTRULEDETAIL_H
#define QLOG_UI_KSTHIGHLIGHTRULEDETAIL_H

#include <QDialog>

namespace Ui {
class KSTHighlightRuleDetail;
}

class KSTHighlightRuleDetail : public QDialog
{
    Q_OBJECT

public:
    explicit KSTHighlightRuleDetail(const QString &ruleName = QString(),
                                    QWidget *parent = nullptr);
    ~KSTHighlightRuleDetail();


public slots:
    void addCondition(int fieldIdx = -1, int operatorId = -1, QString value = QString());
    void save();
    void ruleNameChanged(const QString&);

private:
    Ui::KSTHighlightRuleDetail *ui;
    QStringList ruleNamesList;
    int condCount;

    void loadRule(const QString &ruleName);
    bool ruleExists(const QString &);
};

#endif // QLOG_UI_KSTHIGHLIGHTRULEDETAIL_H
