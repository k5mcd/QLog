#include <QDebug>
#include <QColor>
#include <QSettings>
#include <QRegExpValidator>
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

MODULE_IDENTIFICATION("qlog.ui.dxwidget");

int DxTableModel::rowCount(const QModelIndex&) const {
    return dxData.count();
}

int DxTableModel::columnCount(const QModelIndex&) const {
    return 9;
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
    /*else if (index.column() == 1 && role == Qt::TextColorRole) {
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

    default: return QVariant();
    }
}

void DxTableModel::addEntry(DxSpot entry) {
    //beginInsertRows(QModelIndex(), dxData.count(), dxData.count());
    beginInsertRows(QModelIndex(), 0, 0);
    //dxData.append(entry);
    dxData.prepend(entry);
    endInsertRows();
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
    ui(new Ui::DxWidget) {

    FCT_IDENTIFICATION;

    QSettings settings;

    socket = nullptr;

    ui->setupUi(this);
    dxTableModel = new DxTableModel(this);

    ui->dxTable->setModel(dxTableModel);
    ui->dxTable->addAction(ui->actionFilter);
    ui->dxTable->hideColumn(6);  //continent
    ui->dxTable->hideColumn(7);  //spotter continen
    ui->dxTable->hideColumn(8);  //band
    ui->dxTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    moderegexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    contregexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    spottercontregexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    bandregexp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    moderegexp.setPattern(modeFilterRegExp());
    contregexp.setPattern(contFilterRegExp());
    spottercontregexp.setPattern(spotterContFilterRegExp());
    bandregexp.setPattern(bandFilterRegExp());

    QStringList DXCservers = settings.value("dxc/servers", QStringList("hamqth.com:7300")).toStringList();
    ui->serverSelect->addItems(DXCservers);
    ui->serverSelect->installEventFilter(new DeleteHighlightedDXServerWhenDelPressedEventFilter);
    QRegExp rx("[^\\:]+:[0-9]{1,5}");
    ui->serverSelect->setValidator(new QRegExpValidator(rx,this));
    QString lastUsedServer = settings.value("dxc/last_server").toString();
    int index = ui->serverSelect->findText(lastUsedServer);
    // if last server still exists then set it otherwise use the first one
    if ( index >= 0 )
    {
        ui->serverSelect->setCurrentIndex(index);
    }
}

void DxWidget::toggleConnect() {
    FCT_IDENTIFICATION;

    if (socket && socket->isOpen()) {
        disconnectCluster();

    }
    else {
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

void DxWidget::connectCluster() {
    FCT_IDENTIFICATION;

    QStringList server = ui->serverSelect->currentText().split(":");
    QString host = server[0];
    int port = server[1].toInt();

    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(receive()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));

    ui->connectButton->setEnabled(false);
    ui->connectButton->setText(tr("Connecting..."));

    ui->log->clear();
    ui->dxTable->clearSelection();
    dxTableModel->clear();
    ui->dxTable->repaint();

    socket->connectToHost(host, port);
}

void DxWidget::disconnectCluster() {
    FCT_IDENTIFICATION;

    ui->commandEdit->setEnabled(false);
    ui->connectButton->setEnabled(true);
    ui->connectButton->setText(tr("Connect"));

    if ( socket )
    {
       socket->disconnect();
       socket->close();

       socket->deleteLater();
       socket = nullptr;
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

void DxWidget::send()
{
    FCT_IDENTIFICATION;

    QByteArray data;
    data.append(ui->commandEdit->text().toLatin1());
    data.append("\r\n");

    if ( socket && socket->isOpen() )
    {
        socket->write(data);
    }

    ui->commandEdit->clear();
    ui->rawCheckBox->setChecked(true);
}

void DxWidget::receive() {
    FCT_IDENTIFICATION;

    static QRegularExpression dxSpotRE("^DX de ([a-zA-Z0-9\\/]+).*:\\s+([0-9|.]+)\\s+([a-zA-Z0-9\\/]+)[^\\s]*\\s+(.*)\\s+(\\d{4}Z)",
                                       QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match;

    QString data(socket->readAll());
    QStringList lines = data.split(QRegExp("(\a|\n|\r)+"));
    foreach (QString line, lines)
    {

        // Skip empty lines
        if ( line.length() == 0 )
        {
            continue;
        }

        if (line.startsWith("login") || line.contains(QRegExp("enter your call(sign)?:"))) {
            QByteArray call = StationProfilesManager::instance()->getCurProfile1().callsign.toLocal8Bit();
            call.append("\r\n");
            socket->write(call);
        }

        if (line.startsWith("DX"))
        {
            match = dxSpotRE.match(line);

            if ( match.hasMatch() )
            {
                QString spotter = match.captured(1);
                QString freq =    match.captured(2);
                QString call =    match.captured(3);
                QString comment = match.captured(4);

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

                emit newSpot(spot);

                if ( spot.mode.contains(moderegexp)
                     && spot.dxcc.cont.contains(contregexp)
                     && spot.dxcc_spotter.cont.contains(spottercontregexp)
                     && spot.band.contains(bandregexp) )
                {
                    emit newFilteredSpot(spot);
                    dxTableModel->addEntry(spot);
                }
            }
        }

        ui->log->appendPlainText(line);
    }
}

void DxWidget::socketError(QAbstractSocket::SocketError socker_error) {
    FCT_IDENTIFICATION;


    QString error_msg = QObject::tr("Cannot connect to DXC Server <p>Reason <b>: ");
    switch (socker_error)
    {
    case QAbstractSocket::ConnectionRefusedError:
        error_msg.append(QObject::tr("Connection Refused"));
        break;
    case QAbstractSocket::RemoteHostClosedError:
        error_msg.append(QObject::tr("Host closed the connection"));
        break;
    case QAbstractSocket::HostNotFoundError:
        error_msg.append(QObject::tr("Host not found"));
        break;
    case QAbstractSocket::SocketTimeoutError:
        error_msg.append(QObject::tr("Timeout"));
        break;
    default:
        error_msg.append(QObject::tr("Internal Error"));

    }
    error_msg.append("</b></p>");

    qInfo() << "Detailed Error: " << socker_error;

    QMessageBox::warning(nullptr, QMessageBox::tr("DXC Server Connection Error"),
                                  error_msg);
    disconnectCluster();
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
    }
#endif
    ui->commandEdit->setEnabled(true);
    ui->connectButton->setEnabled(true);
    ui->connectButton->setText(tr("Disconnect"));

    saveDXCServers();
}

void DxWidget::rawModeChanged() {
    FCT_IDENTIFICATION;

    if (ui->rawCheckBox->isChecked()) {
        ui->stack->setCurrentIndex(1);
    }
    else {
        ui->stack->setCurrentIndex(0);
    }
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

    if ( socket && socket->isOpen() )
    {
        /* reconnect DXC Server */
        if ( index >= 0 )
        {
            disconnectCluster();
            connectCluster();
        }
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
    delete ui;
}
