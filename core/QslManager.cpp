#include <QProgressDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include <QNetworkReply>
#include "QslManager.h"
#include "core/debug.h"
#include "core/Eqsl.h"

MODULE_IDENTIFICATION("qlog.core.qslmanager");

QSLManager::QSLManager(QWidget *parent) :
    QObject(parent),
    parentWidget(parent)
{
    FCT_IDENTIFICATION;
}

void QSLManager::showQSLImage(const QSqlRecord &qso, QSLType type)
{
    FCT_IDENTIFICATION;

    qDebug(function_parameters) << type;

    switch ( type )
    {
    /*******************/
    /* Show EQSL Image */
    /*******************/
    case EQSL_QSL:
    {
        QProgressDialog* dialog = new QProgressDialog(tr("Downloading eQSL Image"),
                                                      tr("Cancel"),
                                                      0, 0,
                                                      parentWidget);
        dialog->setWindowModality(Qt::WindowModal);
        dialog->setRange(0, 0);
        dialog->setAutoClose(true);
        dialog->show();

        EQSL *eQSL = new EQSL(dialog);

        connect(eQSL, &EQSL::QSLImageFound, this, [dialog, eQSL](QString imgFile)
        {
            dialog->done(0);
            QDesktopServices::openUrl(imgFile);
            eQSL->deleteLater();
        });

        connect(eQSL, &EQSL::QSLImageError, this, [dialog, eQSL](QString error)
        {
            dialog->done(1);
            QMessageBox::critical(nullptr,
                                  tr("QLog Error"),
                                  tr("eQSL Download Image failed: ") + error);
            eQSL->deleteLater();
        });

        QNetworkReply* reply = eQSL->getQSLImage(qso);

        connect(dialog, &QProgressDialog::canceled, this, [reply, eQSL]()
        {
            qCDebug(runtime)<< "Operation canceled";
            if ( reply )
            {
               reply->abort();
               reply->deleteLater();
            }
            eQSL->deleteLater();
        });
    }
    break;

    /****************/
    /* Unknown type */
    /****************/
    default:
        qInfo()<<"Unknown QSL Type "<< type;
    }
}
