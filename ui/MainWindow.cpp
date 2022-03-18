#include <QSettings>
#include <QFileDialog>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QLabel>
#include <QColor>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui/SettingsDialog.h"
#include "ui/ImportDialog.h"
#include "ui/ExportDialog.h"
#include "ui/LotwDialog.h"
#include "core/Fldigi.h"
#include "core/Rig.h"
#include "core/Rotator.h"
#include "core/Wsjtx.h"
#include "core/ClubLog.h"
#include "core/Conditions.h"
#include "data/Data.h"
#include "core/debug.h"
#include "ui/NewContactWidget.h"
#include "ui/QSOFilterDialog.h"
#include "ui/Eqsldialog.h"
#include "ui/ClublogDialog.h"
#include "ui/QrzDialog.h"
#include "ui/AwardsDialog.h"
#include "core/Lotw.h"
#include "core/Eqsl.h"
#include "core/QRZ.h"
#include "core/CredentialStore.h"

MODULE_IDENTIFICATION("qlog.ui.mainwindow");

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stats(new StatisticsWidget)
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    QSettings settings;

    // restore the window geometry
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    StationProfile profile = StationProfilesManager::instance()->getCurrent();

    conditionsLabel = new QLabel("", ui->statusBar);
    callsignLabel = new QLabel(profile.callsign.toLower(), ui->statusBar);
    locatorLabel = new QLabel(profile.locator.toLower(), ui->statusBar);
    operatorLabel = new QLabel(profile.operatorName, ui->statusBar);
    darkLightModeSwith = new SwitchButton("", ui->statusBar);
    darkIconLabel = new QLabel("<html><img src=':/icons/light-dark-24px.svg'></html>",ui->statusBar);

    ui->toolBar->hide();
    ui->statusBar->addWidget(callsignLabel);
    ui->statusBar->addWidget(locatorLabel);
    ui->statusBar->addWidget(operatorLabel);
    ui->statusBar->addWidget(conditionsLabel);
    ui->statusBar->addPermanentWidget(darkIconLabel);
    ui->statusBar->addPermanentWidget(darkLightModeSwith);

    connect(darkLightModeSwith, SIGNAL(stateChanged(int)), this, SLOT(darkModeToggle(int)));

    connect(this, &MainWindow::themeChanged, ui->bandmapWidget, &BandmapWidget::update);
    connect(this, &MainWindow::themeChanged, ui->onlineMapWidget, &OnlineMapWidget::changeTheme);
    connect(this, &MainWindow::themeChanged, stats, &StatisticsWidget::changeTheme);

    darkLightModeSwith->setChecked(settings.value("darkmode", false).toBool());
/*
    QMenu* trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(ui->actionQuit);

    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
    trayIcon->showMessage("Hello", "This is a test", QIcon());
*/


    connect(Rig::instance(), SIGNAL(rigErrorPresent(QString)), this, SLOT(rigErrorHandler(QString)));
    connect(Rotator::instance(), SIGNAL(rotErrorPresent(QString)), this, SLOT(rotErrorHandler(QString)));

    Fldigi* fldigi = new Fldigi(this);
    connect(fldigi, &Fldigi::addContact, ui->newContactWidget, &NewContactWidget::saveExternalContact);

    Wsjtx* wsjtx = new Wsjtx(this);
    connect(wsjtx, &Wsjtx::statusReceived, ui->wsjtxWidget, &WsjtxWidget::statusReceived);
    connect(wsjtx, &Wsjtx::decodeReceived, ui->wsjtxWidget, &WsjtxWidget::decodeReceived);
    connect(wsjtx, &Wsjtx::addContact, ui->newContactWidget, &NewContactWidget::saveExternalContact);
    connect(ui->wsjtxWidget, &WsjtxWidget::CQSpot, &networknotification, &NetworkNotification::WSJTXCQSpot);
    connect(ui->wsjtxWidget, &WsjtxWidget::reply, wsjtx, &Wsjtx::startReply);
    connect(this, &MainWindow::settingsChanged, wsjtx, &Wsjtx::reloadSetting);

    //ClubLog* clublog = new ClubLog(this);

    connect(ui->logbookWidget, &LogbookWidget::logbookUpdated, stats, &StatisticsWidget::refreshGraph);
    connect(ui->logbookWidget, &LogbookWidget::contactUpdated, &networknotification, &NetworkNotification::QSOUpdated);
    connect(ui->logbookWidget, &LogbookWidget::contactDeleted, &networknotification, &NetworkNotification::QSODeleted);

    connect(ui->newContactWidget, &NewContactWidget::contactAdded, ui->logbookWidget, &LogbookWidget::updateTable);
    connect(ui->newContactWidget, &NewContactWidget::contactAdded, &networknotification, &NetworkNotification::QSOInserted);

    connect(ui->newContactWidget, &NewContactWidget::newTarget, ui->mapWidget, &MapWidget::setTarget);
    connect(ui->newContactWidget, &NewContactWidget::newTarget, ui->onlineMapWidget, &OnlineMapWidget::setTarget);
    //connect(ui->newContactWidget, &NewContactWidget::contactAdded, clublog, &ClubLog::uploadContact);
    connect(ui->newContactWidget, &NewContactWidget::filterCallsign, ui->logbookWidget, &LogbookWidget::filterCallsign);
    connect(ui->newContactWidget, &NewContactWidget::userFrequencyChanged, ui->bandmapWidget, &BandmapWidget::updateRxFrequency);
    connect(ui->newContactWidget, &NewContactWidget::newStationProfile, this, &MainWindow::stationProfileChanged);
    connect(ui->newContactWidget, &NewContactWidget::newStationProfile, ui->rotatorWidget, &RotatorWidget::redrawMap);
    connect(ui->newContactWidget, &NewContactWidget::markQSO, ui->bandmapWidget, &BandmapWidget::addSpot);

    connect(ui->dxWidget, &DxWidget::newSpot, ui->bandmapWidget, &BandmapWidget::addSpot);
    connect(ui->dxWidget, &DxWidget::newSpot, &networknotification, &NetworkNotification::dxSpot);
    connect(ui->dxWidget, &DxWidget::tuneDx, ui->newContactWidget, &NewContactWidget::tuneDx);

    connect(ui->bandmapWidget, &BandmapWidget::tuneDx, ui->newContactWidget, &NewContactWidget::tuneDx);

    connect(ui->wsjtxWidget, &WsjtxWidget::showDxDetails, ui->newContactWidget, &NewContactWidget::showDx);

    conditions = new Conditions(this);
    connect(conditions, &Conditions::conditionsUpdated, this, &MainWindow::conditionsUpdated);
    conditions->update();

    ui->newContactWidget->addPropConditions(conditions);

    if ( StationProfilesManager::instance()->profilesList().isEmpty() )
    {
        showSettings();
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    FCT_IDENTIFICATION;

    QSettings settings;

    // save the window geometry
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    if ( stats )
    {
        stats->close();
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::rigConnect() {
    FCT_IDENTIFICATION;

    if ( ui->actionConnectRig->isChecked() )
    {
        Rig::instance()->open();
    }
    else
    {
        Rig::instance()->close();
    }
}

void MainWindow::rigErrorHandler(const QString &error)
{
    FCT_IDENTIFICATION;

    QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                          QMessageBox::tr("Rig Error: <p>") + error +"</p>");
    ui->actionConnectRig->setChecked(false);
}

void MainWindow::rotErrorHandler(const QString &error)
{
    FCT_IDENTIFICATION;

    QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                          QMessageBox::tr("Rotator Error: <p>") + error +"</p>");
    ui->actionConnectRotator->setChecked(false);
}

void MainWindow::stationProfileChanged()
{
    FCT_IDENTIFICATION;

    StationProfile profile = StationProfilesManager::instance()->getCurrent();

    qCDebug(runtime) << profile.callsign << " " << profile.locator << " " << profile.operatorName;

    callsignLabel->setText(profile.callsign.toLower());
    locatorLabel->setText(profile.locator.toLower());
    operatorLabel->setText(profile.operatorName);

    emit settingsChanged();
}

void MainWindow::darkModeToggle(int mode)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << mode;

    QSettings settings;
    bool darkMode = (mode == Qt::Checked) ? true: false;
    settings.setValue("darkmode", darkMode);

    if ( mode == Qt::Checked)
    {
        setDarkMode();
    }
    else
    {
        setLightMode();
    }

    QFile style(":/res/stylesheet.css");
    style.open(QFile::ReadOnly | QIODevice::Text);
    qApp->setStyleSheet(style.readAll());
    style.close();

    emit themeChanged(darkMode);

}

void MainWindow::setDarkMode()
{
    FCT_IDENTIFICATION;

    QPalette darkPalette;
    QColor darkColor = QColor(45,45,45);
    QColor disabledColor = QColor(127,127,127);
    darkPalette.setColor(QPalette::Window, darkColor);
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(18,18,18));
    darkPalette.setColor(QPalette::AlternateBase, darkColor);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
    darkPalette.setColor(QPalette::Button, darkColor);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

    qApp->setPalette(darkPalette);
}

void MainWindow::setLightMode()
{
    FCT_IDENTIFICATION;

    qApp->setPalette(this->style()->standardPalette());
}

void MainWindow::rotConnect() {
    FCT_IDENTIFICATION;

    if ( ui->actionConnectRotator->isChecked() )
    {
        Rotator::instance()->open();
    }
    else
    {
        Rotator::instance()->close();
    }

}

void MainWindow::showSettings() {
    FCT_IDENTIFICATION;

    SettingsDialog sw;
    if (sw.exec() == QDialog::Accepted) {
        rigConnect();
        rotConnect();
        stationProfileChanged();
        //Do not call settingsChange because stationProfileChanged does it
        //emit settingsChanged();
    }
}

void MainWindow::showStatistics()
{
    FCT_IDENTIFICATION;

    stats->show();
}

void MainWindow::importLog() {
    FCT_IDENTIFICATION;

    ImportDialog dialog;
    dialog.exec();
    ui->logbookWidget->updateTable();
}

void MainWindow::exportLog() {
    FCT_IDENTIFICATION;

    ExportDialog dialog;
    dialog.exec();
}

void MainWindow::showLotw()
{
    FCT_IDENTIFICATION;

    if ( ! Lotw::getUsername().isEmpty() )
    {
        LotwDialog dialog;
        dialog.exec();
        ui->logbookWidget->updateTable();
    }
    else
    {
        QMessageBox::warning(this, tr("QLog Warning"), tr("LoTW is not configured properly.<p> Please, use <b>Settings</b> dialog to configure it.</p>"));
    }
}

void MainWindow::showeQSL()
{
    FCT_IDENTIFICATION;

    if ( ! EQSL::getUsername().isEmpty() )
    {
        EqslDialog dialog;
        dialog.exec();
        ui->logbookWidget->updateTable();
    }
    else
    {
        QMessageBox::warning(this, tr("QLog Warning"), tr("eQSL is not configured properly.<p> Please, use <b>Settings</b> dialog to configure it.</p>"));
    }
}

void MainWindow::showClublog()
{
    FCT_IDENTIFICATION;

    if ( ! ClubLog::getEmail().isEmpty() )
    {
        ClublogDialog dialog;
        dialog.exec();
        ui->logbookWidget->updateTable();
    }
    else
    {
        QMessageBox::warning(this, tr("QLog Warning"), tr("Clublog is not configured properly.<p> Please, use <b>Settings</b> dialog to configure it.</p>"));
    }
}

void MainWindow::showQRZ()
{
    FCT_IDENTIFICATION;

    QString logbookAPIKey = QRZ::getLogbookAPIKey();

    if ( !logbookAPIKey.isEmpty() )
    {
        QRZDialog dialog;
        dialog.exec();
        ui->logbookWidget->updateTable();
    }
    else
    {
        QMessageBox::warning(this, tr("QLog Warning"), tr("QRZ.com is not configured properly.<p> Please, use <b>Settings</b> dialog to configure it.</p>"));
    }

}

void MainWindow::showAwards()
{
    FCT_IDENTIFICATION;

    AwardsDialog dialog;
    dialog.exec();
}

void MainWindow::showAbout() {
    FCT_IDENTIFICATION;

    QString aboutText = tr("<h1>QLog %1</h1>"
                        "<p>&copy; 2019 Thomas Gatzweiler DL2IC<br/>"
                        "&copy; 2021-2022 Ladislav Foldyna OK1MLG</p>"
                        "<p>Icon by <a href='http://www.iconshock.com'>Icon Shock</a><br />"
                        "Satellite images by <a href='http://www.nasa.gov'>NASA</p>");


    QString version = QCoreApplication::applicationVersion();
    aboutText = aboutText.arg(version);

    QMessageBox::about(this, tr("About"), aboutText);
}

void MainWindow::conditionsUpdated() {
    FCT_IDENTIFICATION;

    QString kcolor, fluxcolor, acolor;

    QString k_index_string, flux_string, a_index_string;

    k_index_string = flux_string = a_index_string = tr("N/A");

    /* https://3fs.net.au/making-sense-of-solar-indices/ */
    if ( conditions->isKIndexValid() )
    {
        double k_index = conditions->getKIndex();

        if (k_index < 3.5) {
            kcolor = "green";
        }
        else if (k_index < 4.5) {
            kcolor = "orange";
        }
        else {
            kcolor = "red";
        }

        k_index_string = QString::number(k_index, 'g', 2);
    }

    if ( conditions->isFluxValid() )
    {
        if ( conditions->getFlux() < 100 )
        {
            fluxcolor = "red";
        }
        else if ( conditions->getFlux() < 200 )
        {
            fluxcolor = "orange";
        }
        else
        {
            fluxcolor = "green";
        }

        flux_string = QString::number(conditions->getFlux());

    }

    if ( conditions->isAIndexValid() )
    {
        if ( conditions->getAIndex() < 27 )
        {
            acolor = "green";
        }
        else if ( conditions->getFlux() < 48 )
        {
            acolor = "orange";
        }
        else
        {
            acolor = "red";
        }

        a_index_string = QString::number(conditions->getAIndex());
    }

    conditionsLabel->setTextFormat(Qt::RichText);
    conditionsLabel->setText(QString("SFI <b style='color: %1'>%2</b> A <b style='color: %3'>%4</b> K <b style='color: %5'>%6</b>").arg(
                                 fluxcolor, flux_string, acolor, a_index_string, kcolor, k_index_string ));
}

void MainWindow::QSOFilterSetting()
{
    FCT_IDENTIFICATION;

    QSOFilterDialog dialog(this);
    dialog.exec();
    ui->logbookWidget->updateTable();
}

MainWindow::~MainWindow() {
    FCT_IDENTIFICATION;

    stats->deleteLater();
    conditions->deleteLater();
    conditionsLabel->deleteLater();
    callsignLabel->deleteLater();
    locatorLabel->deleteLater();
    operatorLabel->deleteLater();
    delete ui;
}
