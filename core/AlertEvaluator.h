#ifndef ALERTEVALUATOR_H
#define ALERTEVALUATOR_H

#include <QObject>
#include "data/DxSpot.h"
#include "data/WsjtxEntry.h"
#include "data/UserAlert.h"



class AlertEvaluator : public QObject
{
    Q_OBJECT
public:
    explicit AlertEvaluator(QObject *parent = nullptr);

public slots:
    void dxSpot(const DxSpot&);
    void WSJTXCQSpot(const WsjtxEntry&);

signals:
    void alert(UserAlert alert);
};

#endif // ALERTEVALUATOR_H
