#include <QDebug>
#include <QSortFilterProxyModel>
#include <QScrollBar>
#include <QMutableListIterator>

#include "WsjtxWidget.h"
#include "ui_WsjtxWidget.h"
#include "data/Data.h"
#include "core/debug.h"
#include "core/Rig.h"
#include "data/StationProfile.h"
#include "ui/ColumnSettingDialog.h"
#include "ui/WsjtxFilterDialog.h"
#include "core/Gridsquare.h"

MODULE_IDENTIFICATION("qlog.ui.wsjtxswidget");

WsjtxWidget::WsjtxWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WsjtxWidget),
    lastSelectedCallsign(QString())
{
    FCT_IDENTIFICATION;

    ui->setupUi(this);

    wsjtxTableModel = new WsjtxTableModel(this);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(wsjtxTableModel);

    ui->tableView->setModel(proxyModel);
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->addAction(ui->actionFilter);
    ui->tableView->addAction(ui->actionDisplayedColumns);
    restoreTableHeaderState();

    contregexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    contregexp.setPattern(contFilterRegExp());
    dxccStatusFilter = dxccStatusFilterValue();
    distanceFilter = getDistanceFilterValue();
    snrFilter = getSNRFilterValue();
    QStringList tmp = dxMemberList();
    dxMemberFilter = QSet<QString>(tmp.begin(), tmp.end());
}

void WsjtxWidget::decodeReceived(WsjtxDecode decode)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<decode.message;

    static QRegularExpression cqRE("^CQ (DX |TEST |[A-Z]{0,2} )?([A-Z0-9\\/]+) ?([A-Z]{2}[0-9]{2})?.*");

    StationProfile profile = StationProfilesManager::instance()->getCurProfile1();

    if ( decode.message.startsWith("CQ") )
    {
        QRegularExpressionMatch match = cqRE.match((decode.message));

        if (  match.hasMatch() )
        {
            WsjtxEntry entry;

            entry.decode = decode;
            entry.callsign = match.captured(2);
            entry.grid = match.captured(3);
            entry.dxcc = Data::instance()->lookupDxcc(entry.callsign);
            entry.status = Data::instance()->dxccStatus(entry.dxcc.dxcc, band, status.mode);
            entry.receivedTime = QDateTime::currentDateTimeUtc();
            entry.freq = currFreq;
            entry.band = band;
            entry.decodedMode = currMode;
            entry.spotter = profile.callsign.toUpper();
            entry.dxcc_spotter = Data::instance()->lookupDxcc(entry.spotter);
            entry.distance = 0.0;
            entry.callsign_member = MembershipQE::instance()->query(entry.callsign);

            if ( !profile.locator.isEmpty() )
            {
                Gridsquare myGrid(profile.locator);
                double distance;

                if ( myGrid.distanceTo(Gridsquare(entry.grid), distance) )
                {
                    entry.distance = round(distance);
                }
            }

            emit CQSpot(entry);

            if ( entry.dxcc.cont.contains(contregexp)
                 && ( entry.status & dxccStatusFilter )
                 && entry.distance > distanceFilter
                 && entry.decode.snr > snrFilter
                 && ( dxMemberFilter.size() == 0
                      || (dxMemberFilter.size() && entry.memberList2Set().intersects(dxMemberFilter)))
               )
            {
                wsjtxTableModel->addOrReplaceEntry(entry);
            }
        }
    }
    else
    {
        QStringList decodedElements = decode.message.split(" ");

        if ( decodedElements.count() > 1 )
        {
            QString callsign = decode.message.split(" ").at(1);
            WsjtxEntry entry;
            entry.callsign = callsign;
            if ( wsjtxTableModel->callsignExists(entry) )
            {
                entry.dxcc = Data::instance()->lookupDxcc(entry.callsign);
                entry.status = Data::instance()->dxccStatus(entry.dxcc.dxcc, band, status.mode);
                entry.decode = decode;
                entry.receivedTime = QDateTime::currentDateTimeUtc();
                entry.freq = currFreq;
                entry.band = band;
                entry.decodedMode = currMode;
                entry.spotter = profile.callsign.toUpper();
                entry.dxcc_spotter = Data::instance()->lookupDxcc(entry.spotter);
                entry.distance = 0.0;
                // it is not needed to update entry.callsign_clubs because addOrReplaceEntry does not
                // update it. Only CQ provides the club membeship info

                wsjtxTableModel->addOrReplaceEntry(entry);
            }
        }
    }

    wsjtxTableModel->spotAging();
    proxyModel->sort(4, Qt::DescendingOrder);

    ui->tableView->repaint();

    setSelectedCallsign(lastSelectedCallsign);
}

void WsjtxWidget::statusReceived(WsjtxStatus newStatus)
{
    FCT_IDENTIFICATION;

    if (this->status.dial_freq != newStatus.dial_freq) {
        currFreq = Hz2MHz(newStatus.dial_freq);
        band = Data::instance()->band(currFreq).name;
        ui->freqLabel->setText(QString("%1 MHz").arg(QSTRING_FREQ(currFreq)));
        wsjtxTableModel->clear();
    }

    if ( this->status.dx_call != newStatus.dx_call )
    {
        lastSelectedCallsign = newStatus.dx_call;
        setSelectedCallsign(lastSelectedCallsign);
        emit showDxDetails(newStatus.dx_call, newStatus.dx_grid);
    }

    if ( this->status.mode != newStatus.mode )
    {
        ui->modeLabel->setText(newStatus.mode);
        currMode = newStatus.mode;
        wsjtxTableModel->setCurrentSpotPeriod(Wsjtx::modePeriodLenght(newStatus.mode)); /*currently, only Status has a correct Mode in the message */
    }

    status = newStatus;
    wsjtxTableModel->spotAging();
    ui->tableView->repaint();
}

void WsjtxWidget::tableViewDoubleClicked(QModelIndex index)
{
    FCT_IDENTIFICATION;

    QModelIndex source_index = proxyModel->mapToSource(index);
    QString callsign = wsjtxTableModel->getCallsign(source_index);
    QString grid = wsjtxTableModel->getGrid(source_index);
    emit showDxDetails(callsign, grid);
    emit reply(wsjtxTableModel->getDecode(source_index));
}

void WsjtxWidget::tableViewClicked(QModelIndex index)
{
    FCT_IDENTIFICATION;

    QModelIndex source_index = proxyModel->mapToSource(index);
    lastSelectedCallsign = wsjtxTableModel->getCallsign(source_index);
}

void WsjtxWidget::setSelectedCallsign(const QString &selectCallsign)
{
    FCT_IDENTIFICATION;

    QModelIndexList nextMatches = proxyModel->match(proxyModel->index(0,0), Qt::DisplayRole, selectCallsign, 1);

    if ( nextMatches.size() >= 1 )
    {
        ui->tableView->setCurrentIndex(nextMatches.at(0));
    }
}

void WsjtxWidget::displayedColumns()
{
    FCT_IDENTIFICATION;

    ColumnSettingSimpleDialog dialog(ui->tableView);
    dialog.exec();
    saveTableHeaderState();
}

void WsjtxWidget::actionFilter()
{
    FCT_IDENTIFICATION;

    WsjtxFilterDialog dialog;

    if (dialog.exec() == QDialog::Accepted)
    {
        dxccStatusFilter = dxccStatusFilterValue();
        contregexp.setPattern(contFilterRegExp());
        distanceFilter = getDistanceFilterValue();
        snrFilter = getSNRFilterValue();
        QStringList tmp = dxMemberList();
        dxMemberFilter = QSet<QString>(tmp.begin(), tmp.end());
        wsjtxTableModel->clear();
    }
}

uint WsjtxWidget::dxccStatusFilterValue()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    return settings.value("wsjtx/filter_dxcc_status", DxccStatus::All).toUInt();
}

QString WsjtxWidget::contFilterRegExp()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    return settings.value("wsjtx/filter_cont_regexp","NOTHING|AF|AN|AS|EU|NA|OC|SA").toString();
}

int WsjtxWidget::getDistanceFilterValue()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    return settings.value("wsjtx/filter_distance", 0).toInt();
}

int WsjtxWidget::getSNRFilterValue()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    return settings.value("wsjtx/filter_snr", 0).toInt();
}

QStringList WsjtxWidget::dxMemberList()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    return settings.value("wsjtx/filter_dx_member_list", false).toStringList();
}

void WsjtxWidget::saveTableHeaderState()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    QByteArray state = ui->tableView->horizontalHeader()->saveState();
    settings.setValue("wsjtx/state", state);
}

void WsjtxWidget::restoreTableHeaderState()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    QVariant state = settings.value("wsjtx/state");

    if (!state.isNull())
    {
        ui->tableView->horizontalHeader()->restoreState(state.toByteArray());
    }
}

WsjtxWidget::~WsjtxWidget()
{
    FCT_IDENTIFICATION;

    saveTableHeaderState();
    delete ui;
}
