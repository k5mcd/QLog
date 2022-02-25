#ifndef QSLMANAGER_H
#define QSLMANAGER_H
#include <QObject>
#include <QSqlRecord>

class QSLManager : public QObject
{
    Q_OBJECT

public:

    enum QSLType
    {
        PAPER_QSL = 1,
        EQSL_QSL = 2,
        NO_QSL = 3
    };
    explicit QSLManager( QWidget * );

    void showQSLImage( const QSqlRecord &qso, QSLType type);

private:
    QWidget *parentWidget;
};

#endif // QSLMANAGER_H
