#include <QSettings>
#include <QFileDialog>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QLabel>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui/SettingsDialog.h"
#include "ui/ImportDialog.h"
#include "ui/ExportDialog.h"
#include "ui/LotwDialog.h"
#include "ui/StatisticsWidget.h"
#include "core/Fldigi.h"
#include "core/Rig.h"
#include "core/Rotator.h"
#include "core/Wsjtx.h"
#include "core/ClubLog.h"
#include "core/Conditions.h"
#include "data/Data.h"
#include "core/debug.h"
#include "ui/NewContactWidget.h"

MODULE_IDENTIFICATION("qlog.ui.mainwindow");

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

    ui->toolBar->hide();
    ui->statusBar->addWidget(callsignLabel);
    ui->statusBar->addWidget(locatorLabel);
    ui->statusBar->addWidget(operatorLabel);
    ui->statusBar->addWidget(conditionsLabel);

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
    connect(fldigi, SIGNAL(contactAdded()), ui->logbookWidget, SLOT(updateTable()));

    Wsjtx* wsjtx = new Wsjtx(this);
    connect(wsjtx, &Wsjtx::statusReceived, ui->wsjtxWidget, &WsjtxWidget::statusReceived);
    connect(wsjtx, &Wsjtx::decodeReceived, ui->wsjtxWidget, &WsjtxWidget::decodeReceived);
    connect(wsjtx, &Wsjtx::contactAdded, ui->logbookWidget, &LogbookWidget::updateTable);
    connect(ui->wsjtxWidget, &WsjtxWidget::reply, wsjtx, &Wsjtx::startReply);

    ClubLog* clublog = new ClubLog(this);
    connect(wsjtx, &Wsjtx::contactAdded, clublog, &ClubLog::uploadContact);

    connect(ui->newContactWidget, &NewContactWidget::contactAdded, ui->logbookWidget, &LogbookWidget::updateTable);
    connect(ui->newContactWidget, &NewContactWidget::newTarget, ui->mapWidget, &MapWidget::setTarget);
    connect(ui->newContactWidget, &NewContactWidget::contactAdded, clublog, &ClubLog::uploadContact);
    connect(ui->newContactWidget, &NewContactWidget::filterCallsign, ui->logbookWidget, &LogbookWidget::filterCallsign);
    connect(ui->newContactWidget, &NewContactWidget::userFrequencyChanged, ui->bandmapWidget, &BandmapWidget::updateRxFrequency);
    connect(ui->newContactWidget, &NewContactWidget::newStationProfile, this, &MainWindow::stationProfileChanged);

    connect(ui->dxWidget, &DxWidget::newSpot, ui->bandmapWidget, &BandmapWidget::addSpot);
    connect(ui->dxWidget, &DxWidget::tuneDx, ui->newContactWidget, &NewContactWidget::tuneDx);

    conditions = new Conditions(this);
    connect(conditions, &Conditions::conditionsUpdated, this, &MainWindow::conditionsUpdated);
    conditions->update();
    ui->newContactWidget->addPropConditions(conditions);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    FCT_IDENTIFICATION;

    QSettings settings;

    // save the window geometry
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

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

void MainWindow::rigErrorHandler(QString error)
{
    FCT_IDENTIFICATION;

    QMessageBox::warning(nullptr, QMessageBox::tr("QLog Warning"),
                          QMessageBox::tr("Rig Error: <p>") + error +"</p>");
    ui->actionConnectRig->setChecked(false);
}

void MainWindow::rotErrorHandler(QString error)
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
        emit settingsChanged();
    }
}

void MainWindow::showStatistics() {
    FCT_IDENTIFICATION;

    StatisticsWidget* stats = new StatisticsWidget();
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

void MainWindow::showLotw() {
    FCT_IDENTIFICATION;

    LotwDialog dialog;
    dialog.exec();
    ui->logbookWidget->updateTable();
}

void MainWindow::showAbout() {
    FCT_IDENTIFICATION;

    QString aboutText = "<h1>QLog %1</h1>"
                        "<p>&copy; 2019 Thomas Gatzweiler DL2IC<br/>"
                        "&copy; 2021 Ladislav Foldyna OK1MLG</p>"
                        "<p>Icon by <a href='http://www.iconshock.com'>Icon Shock</a><br />"
                        "Satellite images by <a href='http://www.nasa.gov'>NASA</p>";


    QString version = QCoreApplication::applicationVersion();
    aboutText = aboutText.arg(version);

    QMessageBox::about(this, "About", aboutText);
}

void MainWindow::conditionsUpdated() {
    FCT_IDENTIFICATION;

    QString kcolor;
    QString k_index_string, flux_string;

    k_index_string = flux_string = tr("N/A");

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
        flux_string = QString::number(conditions->getFlux());
    }

    conditionsLabel->setTextFormat(Qt::RichText);
    conditionsLabel->setText(QString("SFI <b>%1</B> K <b style='color: %2'>%3</b>").arg(
                             flux_string, kcolor, k_index_string ));
}

MainWindow::~MainWindow() {
    FCT_IDENTIFICATION;

    delete ui;
}
