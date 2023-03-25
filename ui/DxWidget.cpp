#include <QDebug>
#include <QColor>
#include <QSettings>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QFontMetrics>
#ifdef Q_OS_WIN
#include <Ws2tcpip.h>
#include <winsock2.h>
#include <Mstcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

#include "DxWidget.h"
#include "ui_DxWidget.h"
#include "data/Data.h"
#include "DxFilterDialog.h"
#include "models/SqlListModel.h"
#include "ui/StyleItemDelegate.h"
#include "core/debug.h"
#include "data/StationProfile.h"
#include "data/WCYSpot.h"
#include "data/WWVSpot.h"
#include "data/ToAllSpot.h"
#include "ui/ColumnSettingDialog.h"

#define CONSOLE_VIEW 4
#define NUM_OF_RECONNECT_ATTEMPTS 3
#define RECONNECT_TIMEOUT 10000

MODULE_IDENTIFICATION("qlog.ui.dxwidget");

int DxTableModel::rowCount(const QModelIndex&) const {
    return dxData.count();
}

int DxTableModel::columnCount(const QModelIndex&) const {
    return 10;
}

QVariant DxTableModel::data(const QModelIndex& index, int role) const
{
    QLocale locale;

    if (role == Qt::DisplayRole) {
        DxSpot spot = dxData.at(index.row());
        switch (index.column()) {
        case 0:
            return spot.time.toString(locale.timeFormat(QLocale::LongFormat)).remove("UTC");
        case 1:
            return spot.callsign;
        case 2:
            return QString::number(spot.freq, 'f', 4);
        case 3:
            return spot.mode;
        case 4:
            return spot.spotter;
        case 5:
            return spot.comment;
        case 6:
            return spot.dxcc.cont;
        case 7:
            return spot.dxcc_spotter.cont;
        case 8:
            return spot.band;
        case 9:
            return spot.memberList2StringList().join(", ");
        default:
            return QVariant();
        }
    }
    else if (index.column() == 1 && role == Qt::BackgroundRole) {
        DxSpot spot = dxData.at(index.row());
        return Data::statusToColor(spot.status, QColor(Qt::transparent));
    }
    else if (index.column() == 1 && role == Qt::ToolTipRole) {
        DxSpot spot = dxData.at(index.row());
        return spot.dxcc.country + " [" + Data::statusToText(spot.status) + "]";
    }
    /*else if (index.column() == 1 && role == Qt::ForegroundRole) {
        DxSpot spot = dxData.at(index.row());
        return Data::statusToInverseColor(spot.status, QColor(Qt::black));
    }*/

    return QVariant();
}

QVariant DxTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    switch (section) {
    case 0: return tr("Time");
    case 1: return tr("Callsign");
    case 2: return tr("Frequency");
    case 3: return tr("Mode");
    case 4: return tr("Spotter");
    case 5: return tr("Comment");
    case 6: return tr("Continent");
    case 7: return tr("Spotter Continent");
    case 8: return tr("Band");
    case 9: return tr("Member");

    default: return QVariant();
    }
}

bool DxTableModel::addEntry(DxSpot entry, bool deduplicate,
                            qint16 dedup_interval, double dedup_freq_tolerance)
{
    bool shouldInsert = true;

    if ( deduplicate )
    {
        for (auto record : qAsConst(dxData))
        {
            if ( record.time.secsTo(entry.time) > dedup_interval )
                break;

            if ( record.callsign == entry.callsign
                 && qAbs(record.freq - entry.freq) < dedup_freq_tolerance )
            {
                qCDebug(runtime) << "Duplicate spot" << record.callsign << record.freq <<  entry.callsign << entry.freq;
                shouldInsert = false;
                break;
            }
        }
    }

    if ( shouldInsert )
    {
        beginInsertRows(QModelIndex(), 0, 0);
        dxData.prepend(entry);
        endInsertRows();
    }

    return shouldInsert;
}

QString DxTableModel::getCallsign(const QModelIndex& index) {
    return dxData.at(index.row()).callsign;
}

double DxTableModel::getFrequency(const QModelIndex& index) {
    return dxData.at(index.row()).freq;
}

void DxTableModel::clear() {
    beginResetModel();
    dxData.clear();
    endResetModel();
}

int WCYTableModel::rowCount(const QModelIndex&) const
{
    return wcyData.count();
}

int WCYTableModel::columnCount(const QModelIndex&) const
{
    return 9;
}

QVariant WCYTableModel::data(const QModelIndex& index, int role) const
{
    QLocale locale;

    if ( role == Qt::DisplayRole )
    {
        WCYSpot spot = wcyData.at(index.row());

        switch (index.column()) {
        case 0:
            return spot.time.toString(locale.timeFormat(QLocale::LongFormat)).remove("UTC");
        case 1:
            return spot.KIndex;
        case 2:
            return spot.expK;
        case 3:
            return spot.AIndex;
        case 4:
            return spot.RIndex;
        case 5:
            return spot.SFI;
        case 6:
            return spot.SA;
        case 7:
            return spot.GMF;
        case 8:
            return spot.Au;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant WCYTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    switch (section)
    {
    case 0: return tr("Time");
    case 1: return tr("K");
    case 2: return tr("expK");
    case 3: return tr("A");
    case 4: return tr("R");
    case 5: return tr("SFI");
    case 6: return tr("SA");
    case 7: return tr("GMF");
    case 8: return tr("Au");

    default: return QVariant();
    }
}

void WCYTableModel::addEntry(WCYSpot entry)
{
    beginInsertRows(QModelIndex(), 0, 0);
    wcyData.prepend(entry);
    endInsertRows();
}

void WCYTableModel::clear()
{
    beginResetModel();
    wcyData.clear();
    endResetModel();
}

int WWVTableModel::rowCount(const QModelIndex&) const
{
    return wwvData.count();
}

int WWVTableModel::columnCount(const QModelIndex&) const
{
    return 5;
}

QVariant WWVTableModel::data(const QModelIndex& index, int role) const
{
    QLocale locale;

    if ( role == Qt::DisplayRole )
    {
        WWVSpot spot = wwvData.at(index.row());

        switch (index.column()) {
        case 0:
            return spot.time.toString(locale.timeFormat(QLocale::LongFormat)).remove("UTC");
        case 1:
            return spot.SFI;
        case 2:
            return spot.AIndex;
        case 3:
            return spot.KIndex;
        case 4:
            return spot.info1 + " " + QChar(0x2192) + " " + spot.info2;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant WWVTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    switch (section)
    {
    case 0: return tr("Time");
    case 1: return tr("SFI");
    case 2: return tr("A");
    case 3: return tr("K");
    case 4: return tr("Info");

    default: return QVariant();
    }
}

void WWVTableModel::addEntry(WWVSpot entry)
{
    beginInsertRows(QModelIndex(), 0, 0);
    wwvData.prepend(entry);
    endInsertRows();
}

void WWVTableModel::clear()
{
    beginResetModel();
    wwvData.clear();
    endResetModel();
}

int ToAllTableModel::rowCount(const QModelIndex&) const
{
    return toAllData.count();
}

int ToAllTableModel::columnCount(const QModelIndex&) const
{
    return 3;
}

QVariant ToAllTableModel::data(const QModelIndex& index, int role) const
{
    QLocale locale;

    if ( role == Qt::DisplayRole )
    {
        ToAllSpot spot = toAllData.at(index.row());

        switch (index.column()) {
        case 0:
            return spot.time.toString(locale.timeFormat(QLocale::LongFormat)).remove("UTC");
        case 1:
            return spot.spotter;
        case 2:
            return spot.message;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant ToAllTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();

    switch (section)
    {
    case 0: return tr("Time");
    case 1: return tr("Spotter");
    case 2: return tr("Message");

    default: return QVariant();
    }
}

void ToAllTableModel::addEntry(ToAllSpot entry)
{
    beginInsertRows(QModelIndex(), 0, 0);
    toAllData.prepend(entry);
    endInsertRows();
}

void ToAllTableModel::clear()
{
    beginResetModel();
    toAllData.clear();
    endResetModel();
}

bool DeleteHighlightedDXServerWhenDelPressedEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    FCT_IDENTIFICATION;

    if ( event->type() == QEvent::KeyPress )
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key::Key_Delete && keyEvent->modifiers() == Qt::ControlModifier)
        {
            auto combobox = dynamic_cast<QComboBox *>(obj);
            if ( combobox )
            {
                combobox->removeItem(combobox->currentIndex());
                combobox->setMinimumWidth(0);
                return true;
            }
        }
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
}

DxWidget::DxWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DxWidget),
    deduplicateSpots(false),
    reconnectAttempts(0)
{
    FCT_IDENTIFICATION;

    QSettings settings;

    socket = nullptr;

    ui->setupUi(this);

    ui->serverSelect->setStyleSheet("QComboBox {color: red}");

    dxTableModel = new DxTableModel(this);
    wcyTableModel = new WCYTableModel(this);
    wwvTableModel = new WWVTableModel(this);
    toAllTableModel = new ToAllTableModel(this);

    ui->dxTable->setModel(dxTableModel);
    ui->dxTable->addAction(ui->actionFilter);
    ui->dxTable->addAction(ui->actionDisplayedColumns);
    ui->dxTable->hideColumn(6);  //continent
    ui->dxTable->hideColumn(7);  //spotter continen
    ui->dxTable->hideColumn(8);  //band
    ui->dxTable->hideColumn(9);  //Memberships
    ui->dxTable->horizontalHeader()->setSectionsMovable(true);

    ui->wcyTable->setModel(wcyTableModel);
    ui->wcyTable->addAction(ui->actionDisplayedColumns);
    ui->wcyTable->horizontalHeader()->setSectionsMovable(true);

    ui->wwvTable->setModel(wwvTableModel);
    ui->wwvTable->addAction(ui->actionDisplayedColumns);
    ui->wwvTable->horizontalHeader()->setSectionsMovable(true);

    ui->toAllTable->setModel(toAllTableModel);
    ui->toAllTable->addAction(ui->actionDisplayedColumns);
    ui->toAllTable->horizontalHeader()->setSectionsMovable(true);

    moderegexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    contregexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    spottercontregexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    bandregexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    moderegexp.setPattern(modeFilterRegExp());
    contregexp.setPattern(contFilterRegExp());
    spottercontregexp.setPattern(spotterContFilterRegExp());
    bandregexp.setPattern(bandFilterRegExp());
    dxccStatusFilter = dxccStatusFilterValue();
    deduplicateSpots = spotDedupValue();
    QStringList tmp = dxMemberList();
    dxMemberFilter = QSet<QString>(tmp.begin(), tmp.end());

    QStringList DXCservers = settings.value("dxc/servers", QStringList("hamqth.com:7300")).toStringList();
    ui->serverSelect->addItems(DXCservers);
    ui->serverSelect->installEventFilter(new DeleteHighlightedDXServerWhenDelPressedEventFilter);
    QRegularExpression rx("[^\\:]+:[0-9]{1,5}");
    ui->serverSelect->setValidator(new QRegularExpressionValidator(rx,this));
    QString lastUsedServer = settings.value("dxc/last_server").toString();
    int index = ui->serverSelect->findText(lastUsedServer);
    // if last server still exists then set it otherwise use the first one
    if ( index >= 0 )
    {
        ui->serverSelect->setCurrentIndex(index);
    }

    QMenu *commandsMenu = new QMenu(this);
    commandsMenu->addAction(ui->actionSpotQSO);
    commandsMenu->addSeparator();
    commandsMenu->addAction(ui->actionShowHFStats);
    commandsMenu->addAction(ui->actionShowVHFStats);
    commandsMenu->addAction(ui->actionShowWCY);
    commandsMenu->addAction(ui->actionShowWWV);
    ui->commandButton->setMenu(commandsMenu);

    reconnectTimer.setInterval(RECONNECT_TIMEOUT);
    reconnectTimer.setSingleShot(true);
    connect(&reconnectTimer, &QTimer::timeout, this, &DxWidget::connectCluster);

    restoreWidgetSetting();
}

void DxWidget::toggleConnect()
{
    FCT_IDENTIFICATION;

    if ( (socket && socket->isOpen())
         || reconnectAttempts )
    {
        disconnectCluster();
    }
    else
    {
        int pos = ui->serverSelect->currentIndex();
        QString curr_server = ui->serverSelect->currentText();
        QValidator::State state = ui->serverSelect->validator()->validate(curr_server,pos);

        if ( state != QValidator::Acceptable )
        {
            QMessageBox::warning(nullptr, QMessageBox::tr("DXC Server Name Error"),
                                          QMessageBox::tr("DXC Server address must be in format<p><b>hostname:port</b> (ex. hamqth.com:7300)</p>"));
            return;
        }
        connectCluster();
    }
}

void DxWidget::connectCluster()
{
    FCT_IDENTIFICATION;

    QStringList server = ui->serverSelect->currentText().split(":");
    QString host = server[0];
    int port = server[1].toInt();

    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &DxWidget::receive);
    connect(socket, &QTcpSocket::connected, this, &DxWidget::connected);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &DxWidget::socketError);
#else
    connect(socket, &QTcpSocket::errorOccurred, this, &DxWidget::socketError);
#endif
    ui->connectButton->setEnabled(false);
    ui->connectButton->setText(tr("Connecting..."));

    if ( reconnectAttempts == 0 )
    {
        ui->log->clear();
        ui->dxTable->clearSelection();
        dxTableModel->clear();
        wcyTableModel->clear();
        wwvTableModel->clear();
        toAllTableModel->clear();
        ui->dxTable->repaint();
    }

    socket->connectToHost(host, port);
}

void DxWidget::disconnectCluster(bool tryReconnect)
{
    FCT_IDENTIFICATION;

    reconnectTimer.stop();
    ui->commandEdit->setEnabled(false);
    ui->commandButton->setEnabled(false);
    ui->connectButton->setEnabled(true);


    if ( socket )
    {
       socket->disconnect();
       socket->close();

       socket->deleteLater();
       socket = nullptr;
    }

    if ( reconnectAttempts < NUM_OF_RECONNECT_ATTEMPTS
         && tryReconnect )
    {
        reconnectAttempts++;
        reconnectTimer.start();
        ui->commandEdit->setPlaceholderText(tr("DX Cluster is temporarily unavailable"));
    }
    else
    {
        reconnectAttempts = 0;
        ui->commandEdit->setPlaceholderText("");
        ui->connectButton->setText(tr("Connect"));
        ui->serverSelect->setStyleSheet("QComboBox {color: red}");
    }
}

void DxWidget::saveDXCServers()
{
    FCT_IDENTIFICATION;

    QSettings settings;

    QStringList serversItems = getDXCServerList();
    settings.setValue("dxc/servers", serversItems);
    settings.setValue("dxc/last_server", ui->serverSelect->currentText());
}

QString DxWidget::modeFilterRegExp()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    QString regexp = "NOTHING";

    if (settings.value("dxc/filter_mode_phone",true).toBool())   regexp = regexp + "|" + Data::MODE_PHONE;
    if (settings.value("dxc/filter_mode_cw",true).toBool())      regexp = regexp + "|" + Data::MODE_CW;
    if (settings.value("dxc/filter_mode_ft8",true).toBool())     regexp = regexp + "|" + Data::MODE_FT8;
    if (settings.value("dxc/filter_mode_digital",true).toBool()) regexp = regexp + "|" + Data::MODE_DIGITAL;

    return regexp;
}

QString DxWidget::contFilterRegExp()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    return settings.value("dxc/filter_cont_regexp","NOTHING|AF|AN|AS|EU|NA|OC|SA").toString();
}

QString DxWidget::spotterContFilterRegExp()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    return settings.value("dxc/filter_spotter_cont_regexp","NOTHING|AF|AN|AS|EU|NA|OC|SA").toString();
}

QString DxWidget::bandFilterRegExp()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    QString regexp = "NOTHING";


    SqlListModel *bands= new SqlListModel("SELECT name FROM bands WHERE enabled = 1 ORDER BY start_freq", "Band");

    int band_index = 1; // the first record (0) is Header - skip it and start at position 1

    while (band_index < bands->rowCount())
    {
        QString band_name = bands->data(bands->index(band_index,0)).toString();
        if ( settings.value("dxc/filter_band_" + band_name,true).toBool() )
        {
            regexp.append("|" + band_name);
        }
        band_index++;
    }
    return regexp;
}

uint DxWidget::dxccStatusFilterValue()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    return settings.value("dxc/filter_dxcc_status", DxccStatus::All).toUInt();
}

bool DxWidget::spotDedupValue()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    return settings.value("dxc/filter_deduplication", false).toBool();
}

QStringList DxWidget::dxMemberList()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    return settings.value("dxc/filter_dx_member_list", false).toStringList();
}

void DxWidget::sendCommand(const QString & command,
                           bool switchToConsole)
{
    FCT_IDENTIFICATION;

    QByteArray data;
    data.append(command.toLatin1());
    data.append("\r\n");

    if ( socket && socket->isOpen() )
    {
        socket->write(data);
    }

    // switch to raw mode to see a response
    if ( switchToConsole )
    {
        ui->viewModeCombo->setCurrentIndex(CONSOLE_VIEW);
    }
}

void DxWidget::saveWidgetSetting()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    QByteArray state = ui->dxTable->horizontalHeader()->saveState();
    settings.setValue("dxc/dxtablestate", state);
    state = ui->wcyTable->horizontalHeader()->saveState();
    settings.setValue("dxc/wcytablestate", state);
    state = ui->wwvTable->horizontalHeader()->saveState();
    settings.setValue("dxc/wwvtablestate", state);
    state = ui->toAllTable->horizontalHeader()->saveState();
    settings.setValue("dxc/toalltablestate", state);

    settings.setValue("dxc/consolefontsize", ui->log->font().pointSize());
}

void DxWidget::restoreWidgetSetting()
{
    FCT_IDENTIFICATION;

    QSettings settings;
    QVariant state = settings.value("dxc/dxtablestate");

    if (!state.isNull())
    {
        ui->dxTable->horizontalHeader()->restoreState(state.toByteArray());
    }

    state = settings.value("dxc/wcytablestate");

    if (!state.isNull())
    {
        ui->wcyTable->horizontalHeader()->restoreState(state.toByteArray());
    }

    state = settings.value("dxc/wwvtablestate");

    if (!state.isNull())
    {
        ui->wwvTable->horizontalHeader()->restoreState(state.toByteArray());
    }
    state = settings.value("dxc/toalltablestate");

    if (!state.isNull())
    {
        ui->toAllTable->horizontalHeader()->restoreState(state.toByteArray());
    }

    int fontsize = settings.value("dxc/consolefontsize", -1).toInt();

    QFont monospace(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    if ( fontsize > 0 )
    {
        monospace.setPointSize(fontsize);
    }
    ui->log->setFont(monospace);
}

void DxWidget::send()
{
    FCT_IDENTIFICATION;

    sendCommand(ui->commandEdit->text());
    ui->commandEdit->clear();
}

void DxWidget::receive()
{
    FCT_IDENTIFICATION;

    static QRegularExpression dxSpotRE("^DX de ([a-zA-Z0-9\\/]+).*:\\s+([0-9|.]+)\\s+([a-zA-Z0-9\\/]+)[^\\s]*\\s+(.*)\\s+(\\d{4}Z)",
                                       QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch dxSpotMatch;

    static QRegularExpression wcySpotRE("^(WCY de) +([A-Z0-9\\-#]*) +<(\\d{2})> *: +K=(\\d{1,3}) expK=(\\d{1,3}) A=(\\d{1,3}) R=(\\d{1,3}) SFI=(\\d{1,3}) SA=([a-zA-Z]{1,3}) GMF=([a-zA-Z]{1,3}) Au=([a-zA-Z]{2}) *$",
                                        QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch wcySpotMatch;

    static QRegularExpression wwvSpotRE("^(WWV de) +([A-Z0-9\\-#]*) +<(\\d{2})Z?> *: *SFI=(\\d{1,3}), A=(\\d{1,3}), K=(\\d{1,3}), (.*\\b) *-> *(.*\\b) *$",
                                        QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch wwvSpotMatch;

    static QRegularExpression toAllSpotRE("^(To ALL de) +([A-Z0-9\\-#]*)\\s?(<(\\d{4})Z>)?[ :]+(.*)?$",
                                        QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch toAllSpotMatch;

    static QRegularExpression splitLineRE("(\a|\n|\r)+");
    static QRegularExpression loginRE("enter your call(sign)?:");

    reconnectAttempts = 0;
    QString data(socket->readAll());
    QStringList lines = data.split(splitLineRE);

    foreach (QString line, lines)
    {

        // Skip empty lines
        if ( line.length() == 0 )
        {
            continue;
        }

        if (line.startsWith("login") || line.contains(loginRE) )
        {
            QByteArray call = StationProfilesManager::instance()->getCurProfile1().callsign.toLocal8Bit();
            call.append("\r\n");
            socket->write(call);
        }

        if ( line.contains("dxspider", Qt::CaseInsensitive) )
        {
            ui->commandButton->setEnabled(true);
        }

        /********************/
        /* Received DX SPOT */
        /********************/
        if ( line.startsWith("DX") )
        {
            dxSpotMatch = dxSpotRE.match(line);

            if ( dxSpotMatch.hasMatch() )
            {
                QString spotter = dxSpotMatch.captured(1);
                QString freq =    dxSpotMatch.captured(2);
                QString call =    dxSpotMatch.captured(3);
                QString comment = dxSpotMatch.captured(4);

                DxccEntity dxcc = Data::instance()->lookupDxcc(call);
                DxccEntity dxcc_spotter = Data::instance()->lookupDxcc(spotter);

                DxSpot spot;

                spot.time =  QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
                spot.callsign = call;
                spot.freq = freq.toDouble() / 1000;
                spot.band = Data::band(spot.freq).name;
                spot.mode = Data::freqToDXCCMode(spot.freq);
                spot.spotter = spotter;
                spot.comment = comment;
                spot.dxcc = dxcc;
                spot.dxcc_spotter = dxcc_spotter;
                spot.status = Data::dxccStatus(spot.dxcc.dxcc, spot.band, Data::freqToDXCCMode(spot.freq));
                spot.callsign_member = MembershipQE::instance()->query(spot.callsign);

                emit newSpot(spot);

                if ( spot.mode.contains(moderegexp)
                     && spot.dxcc.cont.contains(contregexp)
                     && spot.dxcc_spotter.cont.contains(spottercontregexp)
                     && spot.band.contains(bandregexp)
                     && ( spot.status & dxccStatusFilter)
                     && ( dxMemberFilter.size() == 0
                          || (dxMemberFilter.size() && spot.memberList2Set().intersects(dxMemberFilter)))
                    )
                {
                    if ( dxTableModel->addEntry(spot, deduplicateSpots) )
                        emit newFilteredSpot(spot);
                }
            }
        }
        /************************/
        /* Received WCY Info */
        /************************/
        else if ( line.startsWith("WCY de") )
        {
            wcySpotMatch = wcySpotRE.match(line);

            if ( wcySpotMatch.hasMatch() )
            {
                WCYSpot spot;

                spot.time = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
                spot.KIndex = wcySpotMatch.captured(4).toUInt();
                spot.expK = wcySpotMatch.captured(5).toUInt();
                spot.AIndex = wcySpotMatch.captured(6).toUInt();
                spot.RIndex = wcySpotMatch.captured(7).toUInt();
                spot.SFI = wcySpotMatch.captured(8).toUInt();
                spot.SA = wcySpotMatch.captured(9);
                spot.GMF = wcySpotMatch.captured(10);
                spot.Au = wcySpotMatch.captured(11);

                emit newWCYSpot(spot);
                wcyTableModel->addEntry(spot);
            }
        }
        /*********************/
        /* Received WWV Info */
        /*********************/
        else if ( line.startsWith("WWV de") )
        {
            wwvSpotMatch = wwvSpotRE.match(line);

            if ( wwvSpotMatch.hasMatch() )
            {
                WWVSpot spot;

                spot.time = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
                spot.SFI = wwvSpotMatch.captured(4).toUInt();
                spot.AIndex = wwvSpotMatch.captured(5).toUInt();
                spot.KIndex = wwvSpotMatch.captured(6).toUInt();
                spot.info1 = wwvSpotMatch.captured(7);
                spot.info2 = wwvSpotMatch.captured(8);

                emit newWWVSpot(spot);
                wwvTableModel->addEntry(spot);
            }
        }
        /*************************/
        /* Received Generic Info */
        /*************************/
        else if ( line.startsWith("To ALL de") )
        {
            toAllSpotMatch = toAllSpotRE.match(line);

            if ( toAllSpotMatch.hasMatch() )
            {
                ToAllSpot spot;

                spot.time = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
                spot.spotter = toAllSpotMatch.captured(2);
                DxccEntity spotter_info = Data::instance()->lookupDxcc(spot.spotter);
                spot.dxcc_spotter = spotter_info;
                spot.message = toAllSpotMatch.captured(5);

                emit newToAllSpot(spot);
                toAllTableModel->addEntry(spot);
            }
        }
        ui->log->appendPlainText(line);
    }
}

void DxWidget::socketError(QAbstractSocket::SocketError socker_error)
{
    FCT_IDENTIFICATION;

    bool reconectRequested = (reconnectAttempts > 0 ) ? true : false;

    QString error_msg = QObject::tr("Cannot connect to DXC Server <p>Reason <b>: ");

    qCDebug(runtime) << socker_error;

    switch (socker_error)
    {
    case QAbstractSocket::ConnectionRefusedError:
        error_msg.append(QObject::tr("Connection Refused"));
        break;
    case QAbstractSocket::RemoteHostClosedError:
        error_msg.append(QObject::tr("Host closed the connection"));
        reconectRequested = true;
        break;
    case QAbstractSocket::HostNotFoundError:
        error_msg.append(QObject::tr("Host not found"));
        break;
    case QAbstractSocket::SocketTimeoutError:
        error_msg.append(QObject::tr("Timeout"));
        reconectRequested = true;
        break;
    case QAbstractSocket::NetworkError:
        error_msg.append(QObject::tr("Network Error"));
        break;
    default:
        error_msg.append(QObject::tr("Internal Error"));

    }
    error_msg.append("</b></p>");

    qInfo() << "Detailed Error: " << socker_error;

    if ( ! reconectRequested || reconnectAttempts == NUM_OF_RECONNECT_ATTEMPTS)
    {
        QMessageBox::warning(nullptr,
                             QMessageBox::tr("DXC Server Connection Error"),
                             error_msg);
    }
    disconnectCluster(reconectRequested);
}

void DxWidget::connected()
{
    FCT_IDENTIFICATION;

    if ( !socket )
    {
        qWarning() << "Socket is not opened";
        return;
    }

    int fd = socket->socketDescriptor();

#ifdef Q_OS_WIN
    DWORD  dwBytesRet = 0;

    struct tcp_keepalive   alive;    // your options for "keepalive" mode
    alive.onoff = TRUE;              // turn it on
    alive.keepalivetime = 10000;     // delay (ms) between requests, here is 10s, default is 2h (7200000)
    alive.keepaliveinterval = 5000;  // delay between "emergency" ping requests, their number (6) is not configurable
      /* So with this config  socket will send keepalive requests every 30 seconds after last data transaction when everything is ok.
          If there is no reply (wire plugged out) it'll send 6 requests with 5s delay  between them and then close.
          As a result we will get disconnect after approximately 1 min timeout.
       */
    if (WSAIoctl(fd, SIO_KEEPALIVE_VALS, &alive, sizeof(alive), NULL, 0, &dwBytesRet, NULL, NULL) == SOCKET_ERROR) {
           qWarning() << "WSAIotcl(SIO_KEEPALIVE_VALS) failed with err#" <<  WSAGetLastError();
    }
#else
    int enableKeepAlive = 1;
    int maxIdle = 10;
    int count = 3;
    int interval = 10;

    if ( setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive, sizeof(enableKeepAlive)) !=0 )
    {
         qWarning() << "Cannot set keepalive for DXC";
    }
    else
    {
#ifndef Q_OS_MACOS
        if ( setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle)) != 0 )
#else
        if ( setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE, &maxIdle, sizeof(maxIdle)) != 0 )
#endif /* Q_OS_MACOS */
        {
            qWarning() << "Cannot set keepalive idle for DXC";
        }

        if ( setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count)) != 0 )
        {
            qWarning() << "Cannot set keepalive counter for DXC";
        }

        if ( setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval)) != 0 )
        {
            qWarning() << "Cannot set keepalive interval for DXC";
        }

        // TODO: setup TCP_USER_TIMEOUT????
    }
#endif
    ui->commandEdit->setEnabled(true);
    ui->connectButton->setEnabled(true);
    ui->connectButton->setText(tr("Disconnect"));
    ui->commandEdit->setPlaceholderText("");
    ui->serverSelect->setStyleSheet("QComboBox {color: green}");

    saveDXCServers();
}

void DxWidget::viewModeChanged(int index)
{
    FCT_IDENTIFICATION;

    ui->stack->setCurrentIndex(index);
}

void DxWidget::entryDoubleClicked(QModelIndex index) {
    FCT_IDENTIFICATION;

    QString callsign = dxTableModel->getCallsign(index);
    double frequency = dxTableModel->getFrequency(index);
    emit tuneDx(callsign, frequency);
}

void DxWidget::actionFilter()
{
    FCT_IDENTIFICATION;
  DxFilterDialog dialog;

  if (dialog.exec() == QDialog::Accepted)
  {
      moderegexp.setPattern(modeFilterRegExp());
      contregexp.setPattern(contFilterRegExp());
      spottercontregexp.setPattern(spotterContFilterRegExp());
      bandregexp.setPattern(bandFilterRegExp());
      dxccStatusFilter = dxccStatusFilterValue();
      deduplicateSpots = spotDedupValue();
      QStringList tmp = dxMemberList();
      dxMemberFilter = QSet<QString>(tmp.begin(), tmp.end());
  }
}

void DxWidget::adjusteServerSelectSize(QString input)
{
    FCT_IDENTIFICATION;

    qDebug(function_parameters)<< input << input.length();

    QFont f;
    QFontMetrics met(f);

    ui->serverSelect->setMinimumWidth(met.boundingRect(input).width() + 35);
    ui->serverSelect->update();
    ui->serverSelect->repaint();
}

void DxWidget::serverSelectChanged(int index)
{
    FCT_IDENTIFICATION;

    qDebug(function_parameters) << index;

    if ( (socket && socket->isOpen())
         || reconnectAttempts )
    {
        /* reconnect DXC Server */
        if ( index >= 0 )
        {
            disconnectCluster();
            connectCluster();
        }
    }
}

void DxWidget::setLastQSO(QSqlRecord qsoRecord)
{
    FCT_IDENTIFICATION;

    lastQSO = qsoRecord;
}

void DxWidget::actionCommandSpotQSO()
{
    FCT_IDENTIFICATION;

    qCDebug(runtime) << "Last QSO" << lastQSO;

    if ( lastQSO.contains("start_time") )
    {
        //lastQSO is valid record
        if ( lastQSO.contains("freq")
             && lastQSO.contains("callsign") )
        {
            bool ok;
            QString remarks = QInputDialog::getText(this,
                                                 tr("DX Spot"),
                                                 tr("Callsign: ") + "<b>" + lastQSO.value("callsign").toString() + "</b>"
                                                 + " "
                                                 + tr("Frequency: ") + "<b>" + QString::number(lastQSO.value("freq").toDouble(), 'f', 3) + "</b>"
                                                 + "<br>"
                                                 + tr("Remarks:"),
                                                 QLineEdit::Normal,
                                                 QString(),
                                                 &ok,
                                                 Qt::Dialog);
            if ( ok )
            {
                QString command;
                command = "dx " + QString::number(lastQSO.value("freq").toDouble(), 'f', 3)
                          + " "
                          + lastQSO.value("callsign").toString()
                          + " "
                          + remarks;

                sendCommand(command);
                lastQSO.clear();
            }
        }
    }
}

void DxWidget::actionCommandShowHFStats()
{
    FCT_IDENTIFICATION;

    sendCommand("sh/hfstats", true);
}

void DxWidget::actionCommandShowVHFStats()
{
    FCT_IDENTIFICATION;

    sendCommand("sh/vhfstats", true);
}

void DxWidget::actionCommandShowWCY()
{
    FCT_IDENTIFICATION;

    sendCommand("sh/wcy", true);
}

void DxWidget::actionCommandShowWWV()
{
    FCT_IDENTIFICATION;

    sendCommand("sh/wwv", true);
}

void DxWidget::displayedColumns()
{
    FCT_IDENTIFICATION;

    QTableView *view = nullptr;

    switch ( ui->stack->currentIndex() )
    {
    case 0: view = ui->dxTable; break;
    case 1: view = ui->wcyTable; break;
    case 2: view = ui->wwvTable; break;
    case 3: view = ui->toAllTable; break;
    default: view = nullptr;
    }

    if ( view )
    {
        ColumnSettingSimpleDialog dialog(view);
        dialog.exec();
        saveWidgetSetting();
    }
}

QStringList DxWidget::getDXCServerList()
{
    FCT_IDENTIFICATION;

    QStringList ret;

    for ( int index = 0; index < ui->serverSelect->count(); index++ )
    {
        ret << ui->serverSelect->itemText(index);
    }
    return ret;
}

DxWidget::~DxWidget() {
    FCT_IDENTIFICATION;

    saveDXCServers();
    saveWidgetSetting();
    delete ui;
}
