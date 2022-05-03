#ifndef ALERTEVALUATOR_H
#define ALERTEVALUATOR_H

#include <QObject>
#include "data/DxSpot.h"
#include "data/WsjtxEntry.h"
#include "data/UserAlert.h"
#include <QRegularExpression>

class AlertRule : public QObject
{
    Q_OBJECT

public:
    explicit AlertRule(QObject *parent = nullptr);
    ~AlertRule() {};

    bool save();
    bool load(const QString &);
    bool match(const WsjtxEntry &wsjtx) const;
    bool match(const DxSpot & spot) const;
    bool isValid() const;
    operator QString() const;
public:
    QString ruleName;
    bool enabled;
    int sourceMap;
    QString dxCallsign;
    int dxCountry;
    int dxLogStatusMap;
    QString dxContinent;
    QString dxComment;
    QString mode;
    QString band;
    int spotterCountry;
    QString spotterContinent;
private:
    bool ruleValid;
    QRegularExpression callsignRE;
    QRegularExpression commentRE;
};

class AlertEvaluator : public QObject
{
    Q_OBJECT
public:
    explicit AlertEvaluator(QObject *parent = nullptr);
    ~AlertEvaluator() {clearRules();}

    void clearRules();

public slots:
    void dxSpot(const DxSpot&);
    void WSJTXCQSpot(const WsjtxEntry&);
    void loadRules();

signals:
    void alert(UserAlert alert);

private:
    QList<AlertRule *>ruleList;
};

#endif // ALERTEVALUATOR_H
