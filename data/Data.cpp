#include <QJsonDocument>
#include <QSqlQuery>
#include <QSqlError>
#include <QColor>
#include "Data.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.data.data");

Data::Data(QObject *parent) : QObject(parent) {
    FCT_IDENTIFICATION;

    loadContests();
    loadPropagationModes();
    loadLegacyModes();
    loadDxccFlags();
    loadSatModes();
    loadIOTA();
    loadSOTA();
}

Data* Data::instance() {
    FCT_IDENTIFICATION;

    static Data instance;
    return &instance;
}

DxccStatus Data::dxccStatus(int dxcc, const QString &band, const QString &mode) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << dxcc << " " << band << " " << mode;

    QString filter;

    QSettings settings;
    QVariant start = settings.value("dxcc/start");
    if (!start.isNull()) {
        filter = QString("AND start_time >= '%1'").arg(start.toDate().toString("yyyy-MM-dd"));
    }

    QString sql_mode = ":mode ";
    if (mode != Data::MODE_CW && mode != Data::MODE_PHONE && mode != Data::MODE_DIGITAL) {
        sql_mode = "(SELECT modes.dxcc FROM modes WHERE modes.name = :mode LIMIT 1) ";
    }

    QSqlQuery query;

    if ( ! query.prepare("SELECT (SELECT contacts.callsign FROM contacts WHERE dxcc = :dxcc " + filter + " ORDER BY start_time ASC LIMIT 1) as entity,"
                  "(SELECT contacts.callsign FROM contacts WHERE dxcc = :dxcc AND band = :band " + filter + " ORDER BY start_time ASC LIMIT 1) as band,"
                  "(SELECT contacts.callsign FROM contacts INNER JOIN modes ON (modes.name = contacts.mode)"
                  "        WHERE contacts.dxcc = :dxcc AND modes.dxcc = " + sql_mode + filter +
                  "        ORDER BY start_time ASC LIMIT 1) as mode,"
                  "(SELECT contacts.callsign FROM contacts INNER JOIN modes ON (modes.name = contacts.mode)"
                  "        WHERE contacts.dxcc = :dxcc AND modes.dxcc = " + sql_mode + filter +
                  "        AND band = :band ORDER BY start_time ASC LIMIT 1) as slot;") )
    {
        qWarning() << "Cannot prepare Select statement";
        return DxccStatus::Unknown;
    }

    query.bindValue(":dxcc", dxcc);
    query.bindValue(":band", band);
    query.bindValue(":mode", mode);

    if ( ! query.exec() )
    {
        qWarning() << "Cannot execute Select statement" << query.lastError();
        return DxccStatus::Unknown;
    }

    if (query.next()) {
        if (query.value(0).isNull()) {
            return DxccStatus::NewEntity;
        }
        if (query.value(1).isNull()) {
            if (query.value(2).isNull()) {
                return DxccStatus::NewBandMode;
            }
            else {
                return DxccStatus::NewBand;
            }
        }
        if (query.value(2).isNull()) {
            return DxccStatus::NewMode;
        }
        if (query.value(3).isNull()) {
            return DxccStatus::NewSlot;
        }
        else {
            return DxccStatus::Worked;
        }
    }
    else {
        return DxccStatus::Unknown;
    }
}

Band Data::band(double freq) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << freq;

    QSqlQuery query;
    if ( ! query.prepare("SELECT name, start_freq, end_freq FROM bands WHERE :freq BETWEEN start_freq AND end_freq") )
    {
        qWarning() << "Cannot prepare Select statement";
        return Band();
    }
    query.bindValue(0, freq);

    if ( ! query.exec() )
    {
        qWarning() << "Cannot execute select statement" << query.lastError();
        return Band();
    }

    if (query.next()) {
        Band band;
        band.name = query.value(0).toString();
        band.start = query.value(1).toDouble();
        band.end = query.value(2).toDouble();
        return band;
    }
    else {
        return Band();
    }
}

QString Data::freqToBand(double freq)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << freq;

    if (freq <= 2.0 && freq >= 1.8) return "160m";
    else if (freq <= 3.8 && freq >= 3.5) return "80m";
    else if (freq <= 7.5 && freq >= 7.0) return "40m";
    else if (freq <= 10.150 && freq >= 10.1) return"30m";
    else if (freq <= 14.350 && freq >= 14.0) return "20m";
    else if (freq <= 18.168 && freq >= 18.068) return "17m";
    else if (freq <= 21.450 && freq >= 21.000) return "15m";
    else if (freq <= 24.990 && freq >= 24.890) return "12m";
    else if (freq <= 29.700 && freq >= 28.000) return "10m";
    else if (freq <= 52 && freq >= 50) return "6m";
    else if (freq <= 148 && freq >= 144) return "2m";
    else if (freq <= 440 && freq >= 430) return "70cm";
    else return QString();
}

QString Data::freqToMode(double freq)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << freq;

    // 2200m
    if (freq >= 0.1357 && freq <= 0.1378) return Data::MODE_CW;

    // 630m
    else if (freq >= 0.472 && freq <= 0.475) return Data::MODE_CW;
    else if (freq <= 0.479) return Data::MODE_DIGITAL;

    // 160m
    else if (freq >= 1.800 && freq < 1.838) return Data::MODE_CW;
    else if (freq >= 1.838 && freq < 1.840) return Data::MODE_DIGITAL;
    else if (freq == 1.840) return Data::MODE_FT8;
    else if (freq > 1.840 && freq < 2.000) return Data::MODE_LSB;

    // 80m
    else if (freq > 3.500 && freq < 3.573) return Data::MODE_CW;
    else if (freq == 3.573) return Data::MODE_FT8;
    else if (freq > 3.573 && freq < 3.580) return Data::MODE_CW;
    else if (freq >= 3.580 && freq < 3.600) return Data::MODE_DIGITAL;
    else if (freq >= 3.600 && freq < 4.000) return Data::MODE_LSB;

    // 60m
    else if (freq >= 5.3515 && freq <= 5.354) return Data::MODE_CW;
    else if (freq <= 5.500) return Data::MODE_LSB;

    // 40m
    else if (freq >= 7.000 && freq < 7.040) return Data::MODE_CW;
    else if (freq >= 7.040 && freq < 7.050) return Data::MODE_DIGITAL;
    else if (freq >= 7.050 && freq < 7.074) return Data::MODE_LSB;
    else if (freq == 7.074) return Data::MODE_FT8;
    else if (freq > 7.074 && freq < 7.300) return Data::MODE_LSB;

    // 30m
    else if (freq >= 10.100 && freq < 10.136) return Data::MODE_CW;
    else if (freq == 10.136 ) return Data::MODE_FT8;
    else if (freq > 10.136 && freq < 10.140) return Data::MODE_CW;
    else if (freq >= 10.140 && freq < 10.150) return Data::MODE_DIGITAL;

    // 20m
    else if (freq >= 14.000 && freq < 14.070) return Data::MODE_CW;
    else if (freq >= 14.070 && freq < 14.074) return Data::MODE_DIGITAL;
    else if (freq == 14.074) return Data::MODE_FT8;
    else if (freq > 14.074 && freq <= 14.099) return Data::MODE_DIGITAL;
    else if (freq > 14.099 && freq < 14.350) return Data::MODE_USB;

    // 17m
    else if (freq >= 18.068 && freq < 18.095) return Data::MODE_CW;
    else if (freq >= 18.095 && freq < 18.100) return Data::MODE_DIGITAL;
    else if (freq == 18.100 ) return Data::MODE_FT8;
    else if (freq > 18.100 && freq < 18.110) return Data::MODE_DIGITAL;
    else if (freq >= 18.110 && freq < 18.268) return Data::MODE_USB;

    // 15m
    else if (freq >= 21.000 && freq < 21.070) return Data::MODE_CW;
    else if (freq >= 21.070 && freq < 21.074) return Data::MODE_LSB;
    else if (freq == 21.074) return Data::MODE_FT8;
    else if (freq > 21.074 && freq < 21.150) return Data::MODE_DIGITAL;
    else if (freq >= 21.150 && freq < 21.450) return Data::MODE_USB;

    // 12m
    else if (freq >= 24.890 && freq < 24.915) return Data::MODE_CW;
    else if (freq == 24.915 ) return Data::MODE_FT8;
    else if (freq > 24.915 && freq < 24.930) return Data::MODE_DIGITAL;
    else if (freq >= 24.930 && freq < 24.990) return Data::MODE_USB;

    // 10m
    else if (freq >= 28.000 && freq < 28.074) return Data::MODE_CW;
    else if (freq == 28.074) return Data::MODE_FT8;
    else if (freq > 28.074 && freq < 28.190) return Data::MODE_DIGITAL;
    else if (freq >= 28.190 && freq < 29.700) return Data::MODE_USB;

    // 6m
    else if (freq >= 50.000 && freq < 50.100) return Data::MODE_CW;
    else if (freq > 50.100 && freq < 50.313) return Data::MODE_USB;
    else if (freq == 50.313 ) return Data::MODE_FT8;
    else if (freq > 50.313 && freq < 54.000) return Data::MODE_USB;

    // 4m
    else if (freq >=70.000 && freq < 70.100) return Data::MODE_CW;
    else if (freq >=70.100 && freq < 70.250) return Data::MODE_USB;
    else if (freq >=70.2500 && freq < 70.500) return Data::MODE_USB;

    // 2m
    else if (freq >= 144.000 && freq < 144.150) return Data::MODE_CW;
    else if (freq >= 144.150 && freq < 144.174) return Data::MODE_USB;
    else if (freq >= 144.174 && freq <= 144.175) return Data::MODE_FT8;
    else if (freq > 144.175 && freq < 148.000) return Data::MODE_USB;

    else return QString();
}

QColor Data::statusToColor(const DxccStatus &status, const QColor &defaultColor) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << status;

    switch (status) {
        case DxccStatus::NewEntity:
            return QColor(229, 57, 53);
        case DxccStatus::NewBand:
        case DxccStatus::NewMode:
        case DxccStatus::NewBandMode:
            return QColor(76, 175, 80);
        case DxccStatus::NewSlot:
            return QColor(30, 136, 229);
        default:
            return defaultColor;
    }
}

QString Data::statusToText(const DxccStatus &status) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << status;

    switch (status) {
        case DxccStatus::NewEntity:
            return tr("New Entity");
        case DxccStatus::NewBand:
            return tr("New Band");
        case DxccStatus::NewMode:
            return tr("New Mode");
        case DxccStatus::NewBandMode:
            return tr("New Band&Mode");
        case DxccStatus::NewSlot:
            return tr("New Slot");
        case DxccStatus::Worked:
            return tr("Worked");
        default:
            return QString("Unknown");
    }
}

QRegularExpression Data::callsignRegEx()
{
    FCT_IDENTIFICATION;

    return QRegularExpression(callsignRegExString(), QRegularExpression::CaseInsensitiveOption);
}

QString Data::callsignRegExString()
{
    FCT_IDENTIFICATION;

    return QString("^([A-Z0-9]+[\\/])?([A-Z][0-9]|[A-Z]{1,2}|[0-9][A-Z])([0-9]|[0-9]+)([A-Z]+)([\\/][A-Z0-9]+)?");
}

QColor Data::statusToInverseColor(const DxccStatus &status, const QColor &defaultColor) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << status;

    switch (status) {
        case DxccStatus::NewEntity:
            return QColor(Qt::white);
        case DxccStatus::NewBand:
        case DxccStatus::NewMode:
        case DxccStatus::NewBandMode:
            return QColor(Qt::white);
        case DxccStatus::NewSlot:
            return QColor(Qt::black);
        default:
            return defaultColor;
    }
}

QPair<QString, QString> Data::legacyMode(const QString &mode) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << mode;
    return legacyModes.value(mode);
}

void Data::loadContests() {
    FCT_IDENTIFICATION;

    QFile file(":/res/data/contests.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    auto objectList = QJsonDocument::fromJson(data).toVariant().toList();
    for (auto &object : qAsConst(objectList))
    {
        QVariantMap contestData = object.toMap();

        QString id = contestData.value("id").toString();
        QString name = contestData.value("name").toString();

        contests.insert(id, name);
    }
}

void Data::loadPropagationModes() {
    FCT_IDENTIFICATION;

    QFile file(":/res/data/propagation_modes.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    auto objects = QJsonDocument::fromJson(data).toVariant().toList();
    for (auto &object : qAsConst(objects))
    {
        QVariantMap propagationModeData = object.toMap();

        QString id = propagationModeData.value("id").toString();
        QString name = propagationModeData.value("name").toString();

        propagationModes.insert(id, name);
    }
}

void Data::loadLegacyModes() {
    FCT_IDENTIFICATION;

    QFile file(":/res/data/legacy_modes.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    QVariantMap modes = QJsonDocument::fromJson(data).toVariant().toMap();
    auto keys = modes.keys();

    for (auto &key : qAsConst(keys))
    {
        QVariantMap legacyModeData = modes[key].toMap();

        QString mode = legacyModeData.value("mode").toString();
        QString submode = legacyModeData.value("submode").toString();
        QPair<QString, QString> modes = QPair<QString, QString>(mode, submode);

        legacyModes.insert(key, modes);
    }
}

void Data::loadDxccFlags()
{
    FCT_IDENTIFICATION;

    QFile file(":/res/data/dxcc.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    auto objects = QJsonDocument::fromJson(data).toVariant().toList();
    for (auto &object : qAsConst(objects))
    {
        QVariantMap dxccData = object.toMap();

        int id = dxccData.value("id").toInt();
        QString flag = dxccData.value("flag").toString();

        flags.insert(id, flag);
    }
}

void Data::loadSatModes()
{
    FCT_IDENTIFICATION;

    QFile file(":/res/data/sat_modes.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    auto objects = QJsonDocument::fromJson(data).toVariant().toList();
    for (auto &object : qAsConst(objects))
    {
        QVariantMap satModesData = object.toMap();

        QString id = satModesData.value("id").toString();
        QString name = satModesData.value("name").toString();

        satModes.insert(id, name);
    }
}

void Data::loadIOTA()
{
    FCT_IDENTIFICATION;

    QFile file(":/res/data/iota.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    auto objects = QJsonDocument::fromJson(data).toVariant().toList();
    for (auto &object : qAsConst(objects))
    {
        QVariantMap iotaData = object.toMap();

        QString id = iotaData.value("id").toString();
        QString name = iotaData.value("name").toString();

        iotaRef.insert(id, name);
    }
}

void Data::loadSOTA()
{
    FCT_IDENTIFICATION;

    QFile file(":/res/data/sota.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data = file.readAll();

    auto objects = QJsonDocument::fromJson(data).toVariant().toList();
    for (auto &object : qAsConst(objects))
    {
        QVariantMap sotaData = object.toMap();

        QString id = sotaData.value("id").toString();
        QString name = ""; // later - use UTF8 string
        sotaRef.insert(id, name);
    }
}

DxccEntity Data::lookupDxcc(const QString &callsign) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << callsign;

    QSqlQuery query;
    if ( ! query.prepare(
                "SELECT\n"
                "    dxcc_entities.id,\n"
                "    dxcc_entities.name,\n"
                "    dxcc_entities.prefix,\n"
                "    dxcc_entities.cont,\n"
                "    CASE\n"
                "        WHEN (dxcc_prefixes.cqz != 0)\n"
                "        THEN dxcc_prefixes.cqz\n"
                "        ELSE dxcc_entities.cqz\n"
                "    END AS cqz,\n"
                "    CASE\n"
                "        WHEN (dxcc_prefixes.ituz != 0)\n"
                "        THEN dxcc_prefixes.ituz\n"
                "        ELSE dxcc_entities.ituz\n"
                "    END AS ituz\n,"
                "    dxcc_entities.lat,\n"
                "    dxcc_entities.lon,\n"
                "    dxcc_entities.tz\n"
                "FROM dxcc_prefixes\n"
                "INNER JOIN dxcc_entities ON (dxcc_prefixes.dxcc = dxcc_entities.id)\n"
                "WHERE (dxcc_prefixes.prefix = :callsign and dxcc_prefixes.exact = true)\n"
                "    OR (dxcc_prefixes.exact = false and :callsign LIKE dxcc_prefixes.prefix || '%')\n"
                "ORDER BY dxcc_prefixes.prefix\n"
                "DESC LIMIT 1\n"
    ) )
    {
        qWarning() << "Cannot prepare Select statement";
        return DxccEntity();
    }

    query.bindValue(":callsign", callsign);

    if ( ! query.exec() )
    {
        qWarning() << "Cannot execte Select statement" << query.lastError();
        return DxccEntity();
    }

    DxccEntity dxcc;
    if (query.next()) {
        dxcc.dxcc = query.value(0).toInt();
        dxcc.country = query.value(1).toString();
        dxcc.prefix = query.value(2).toString();
        dxcc.cont = query.value(3).toString();
        dxcc.cqz = query.value(4).toInt();
        dxcc.ituz = query.value(5).toInt();
        dxcc.latlon[0] = query.value(6).toDouble();
        dxcc.latlon[1] = query.value(7).toDouble();
        dxcc.tz = query.value(8).toFloat();
        dxcc.flag = flags.value(dxcc.dxcc);
    }
    else {
        dxcc.dxcc = 0;
    }
    return dxcc;
}

QString Data::dxccFlag(int dxcc) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << dxcc;
    return flags.value(dxcc);
}

const QString Data::MODE_CW = "CW";
const QString Data::MODE_DIGITAL = "DIGITAL";
const QString Data::MODE_FT8 = "FT8";
const QString Data::MODE_LSB = "PHONE"; // use just generic label
const QString Data::MODE_USB = "PHONE"; // use just generic label
const QString Data::MODE_PHONE = "PHONE";
