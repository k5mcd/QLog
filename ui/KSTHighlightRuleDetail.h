#ifndef KSTHIGHLIGHTRULEDETAIL_H
#define KSTHIGHLIGHTRULEDETAIL_H

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

#endif // KSTHIGHLIGHTRULEDETAIL_H
