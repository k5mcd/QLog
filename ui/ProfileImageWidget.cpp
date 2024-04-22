#include <QVBoxLayout>
#include "ProfileImageWidget.h"
#include "ui_ProfileImageWidget.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.ui.profileimagewidget");

ProfileImageWidget::ProfileImageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfileImageWidget),
    imageLabel(new AspectRatioLabel(this)),
    nm(new QNetworkAccessManager(this)),
    reply(nullptr)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setMinimumSize(sizeHint());
    layout->addWidget(imageLabel);
}

ProfileImageWidget::~ProfileImageWidget()
{
    FCT_IDENTIFICATION;

    delete ui;
    if ( reply )
    {
        reply->abort();
        reply->deleteLater();
    }
    imageLabel->deleteLater();
    nm->deleteLater();
}

void ProfileImageWidget::loadImageFromUrl(const QString &urlAddress)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << urlAddress;

    if ( urlAddress.isEmpty() )
    {
        imageLabel->clear();
        imageLabel->setToolTip("");
        return;
    }

    if ( reply )
    {
        qCDebug(runtime) << "Previous request is still running";
        reply->abort();
        reply->deleteLater();
    }

    QUrl url(urlAddress);
    QNetworkRequest request(url);

    reply = nm->get(request);
    connect(reply, &QNetworkReply::finished, this, [this]()
    {
        if ( reply->error() == QNetworkReply::NoError )
        {
            QByteArray imageData = reply->readAll();
            QPixmap pixmap;

            pixmap.loadFromData(imageData);
            imageLabel->setPixmap(pixmap);
            imageLabel->setScaledContents(true);
            mimeType = mimeDatabase.mimeTypeForData(imageData);

            if ( mimeType.name().contains("image", Qt::CaseInsensitive))
            {
               // QByteArray->QString in arg is due to QT5.12
               imageLabel->setToolTip(QString("<img src='data:%0;base64, %1'>").arg(mimeType.name(), QString(imageData.toBase64())));
            }
        }
        else
        {
            qCWarning(runtime) << "Profile Image download error:" << reply->errorString();
        }

        reply->deleteLater();
        reply = nullptr;
    });
}

AspectRatioLabel::AspectRatioLabel(QWidget* parent, Qt::WindowFlags f) :
    QLabel(parent, f)
{
}

AspectRatioLabel::~AspectRatioLabel()
{
}

void AspectRatioLabel::setPixmap(const QPixmap& pm)
{
    pixmapWidth = pm.width();
    pixmapHeight = pm.height();

    updateMargins();
    QLabel::setPixmap(pm);
}

void AspectRatioLabel::resizeEvent(QResizeEvent* event)
{
    updateMargins();
    QLabel::resizeEvent(event);
}

void AspectRatioLabel::updateMargins()
{
    if ( pixmapWidth <= 0 || pixmapHeight <= 0 )
        return;

    int w = width();
    int h = height();

    if ( w <= 0 || h <= 0 )
        return;

    if ( w * pixmapHeight > h * pixmapWidth )
    {
        int m = (w - (pixmapWidth * h / pixmapHeight)) / 2;
        setContentsMargins(m, 0, m, 0);
    }
    else
    {
        int m = (h - (pixmapHeight * w / pixmapWidth)) / 2;
        setContentsMargins(0, m, 0, m);
    }
}
