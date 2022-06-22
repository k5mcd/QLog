#include <QSqlRecord>
#include <QtXml>
#include "logformat/AdxFormat.h"
#include "logformat/AdiFormat.h"
#include "core/debug.h"
#include "data/Data.h"

MODULE_IDENTIFICATION("qlog.logformat.adxformat");

void AdxFormat::exportStart() {
    FCT_IDENTIFICATION;

    QString date = QDateTime::currentDateTimeUtc().toString("yyyyMMdd hhmmss");

    writer = new QXmlStreamWriter(stream.device());
    writer->setAutoFormatting(true);

    writer->writeStartDocument();
    writer->writeStartElement("ADX");

    writer->writeStartElement("HEADER");
    writer->writeTextElement("ADIF_VER", "3.1.3");
    writer->writeTextElement("PROGRAMID", "QLog");
    writer->writeTextElement("PROGRAMVERSION", VERSION);
    writer->writeTextElement("CREATED_TIMESTAMP", date);
    writer->writeEndElement();

    writer->writeStartElement("RECORDS");
}

void AdxFormat::exportEnd() {
    FCT_IDENTIFICATION;

    writer->writeEndDocument();
    delete writer;
}

void AdxFormat::importStart()
{
    FCT_IDENTIFICATION;

    reader = new QXmlStreamReader(stream.device());

    while ( reader->readNextStartElement() )
    {
        qCDebug(runtime)<<reader->name();
        if ( reader->name() == "ADX" )
        {
            while ( reader->readNextStartElement() )
            {
                qCDebug(runtime)<<reader->name();
                if ( reader->name() == "HEADER" )
                {
                    reader->skipCurrentElement();
                }
                else if ( reader->name() == "RECORDS" )
                {
                    qCDebug(runtime)<<"records found";
                    /* header is loaded, QLog is currently in Records sections
                       which is loaded by importNext procedure */
                    return;
                }
            }
        }
        else
        {
            reader->skipCurrentElement();
        }
    }
}

void AdxFormat::importEnd()
{
    FCT_IDENTIFICATION;

    delete reader;
}

bool AdxFormat::importNext(QSqlRecord &record)
{
    FCT_IDENTIFICATION;

    /* currently we should be in Records section */

    QMap<QString, QVariant> contact;

    if (!readContact(contact))
    {
        return false;
    }

    /* Set default values if not present */
    if (defaults) {
        auto keys = defaults->keys();
        for (auto &key : qAsConst(keys))
        {
            if (contact.value(key).isNull()) {
                contact.insert(key, defaults->value(key));
            }
        }
    }
    record.setValue("callsign", contact.take("call"));
    record.setValue("rst_rcvd", contact.take("rst_rcvd"));
    record.setValue("rst_sent", contact.take("rst_sent"));
    record.setValue("gridsquare", contact.take("gridsquare").toString().toUpper());
    record.setValue("cqz", contact.take("cqz"));
    record.setValue("ituz", contact.take("ituz"));
    record.setValue("freq", contact.take("freq"));
    record.setValue("band", contact.take("band").toString().toLower());
    record.setValue("cont", contact.take("cont").toString().toUpper());
    record.setValue("dxcc", contact.take("dxcc"));
    record.setValue("pfx", contact.take("pfx").toString().toUpper());
    record.setValue("state", contact.take("state").toString());
    record.setValue("cnty", contact.take("cnty"));
    record.setValue("iota", contact.take("iota").toString().toUpper());
    record.setValue("qsl_rcvd", AdiFormat::parseQslRcvd(contact.take("qsl_rcvd").toString()));
    record.setValue("qsl_rdate", AdiFormat::parseDate(contact.take("qslrdate").toString()));
    record.setValue("qsl_sent", AdiFormat::parseQslSent(contact.take("qsl_sent").toString()));
    record.setValue("qsl_sdate", AdiFormat::parseDate(contact.take("qslsdate").toString()));
    record.setValue("lotw_qsl_rcvd", AdiFormat::parseQslRcvd(contact.take("lotw_qsl_rcvd").toString()));
    record.setValue("lotw_qslrdate", AdiFormat::parseDate(contact.take("lotw_qslrdate").toString()));
    record.setValue("lotw_qsl_sent", AdiFormat::parseQslSent(contact.take("lotw_qsl_sent").toString()));
    record.setValue("lotw_qslsdate", AdiFormat::parseDate(contact.take("lotw_qslsdate").toString()));
    record.setValue("tx_pwr", contact.take("tx_pwr").toDouble());
    record.setValue("age", contact.take("age"));
    record.setValue("a_index", contact.take("a_index"));
    record.setValue("ant_az", contact.take("ant_az"));
    record.setValue("ant_el", contact.take("ant_el"));
    record.setValue("ant_path", contact.take("ant_path"));
    record.setValue("arrl_sect", contact.take("arrl_sect"));
    record.setValue("award_submitted",contact.take("award_submitted"));
    record.setValue("award_granted",contact.take("award_granted"));
    record.setValue("band_rx",contact.take("band_rx").toString().toLower());
    record.setValue("check",contact.take("check"));
    record.setValue("class",contact.take("class"));
    record.setValue("clublog_qso_upload_date",AdiFormat::parseDate(contact.take("clublog_qso_upload_date").toString()));
    record.setValue("clublog_qso_upload_status",AdiFormat::parseUploadStatus(contact.take("clublog_qso_upload_status").toString()));
    record.setValue("contacted_op",contact.take("contacted_op"));
    record.setValue("contest_id",contact.take("contest_id"));
    record.setValue("credit_submitted",contact.take("credit_submitted"));
    record.setValue("credit_granted",contact.take("credit_granted"));
    record.setValue("darc_dok",contact.take("darc_dok"));
    record.setValue("distance",contact.take("distance"));
    record.setValue("email",contact.take("email"));
    record.setValue("eq_call",contact.take("eq_call"));
    record.setValue("eqsl_qslrdate",AdiFormat::parseDate(contact.take("eqsl_qslrdate").toString()));
    record.setValue("eqsl_qslsdate",AdiFormat::parseDate(contact.take("eqsl_qslsdate").toString()));
    record.setValue("eqsl_qsl_rcvd",AdiFormat::parseQslRcvd(contact.take("eqsl_qsl_rcvd").toString()));
    record.setValue("eqsl_qsl_sent",AdiFormat::parseQslSent(contact.take("eqsl_qsl_sent").toString()));
    record.setValue("fists",contact.take("fists"));
    record.setValue("fists_cc",contact.take("fists_cc"));
    record.setValue("force_init",contact.take("force_init").toString());
    record.setValue("freq_rx",contact.take("freq_rx"));
    record.setValue("guest_op",contact.take("guest_op"));
    record.setValue("hrdlog_qso_upload_date",AdiFormat::parseDate(contact.take("hrdlog_qso_upload_date").toString()));
    record.setValue("hrdlog_qso_upload_status",AdiFormat::parseUploadStatus(contact.take("hrdlog_qso_upload_status").toString()));
    record.setValue("iota_island_id",contact.take("iota_island_id").toString().toUpper());
    record.setValue("k_index",contact.take("k_index"));
    record.setValue("lat",contact.take("lat"));
    record.setValue("lon",contact.take("lon"));
    record.setValue("max_bursts",contact.take("max_bursts"));
    record.setValue("ms_shower",contact.take("ms_shower"));
    record.setValue("my_arrl_sect",contact.take("my_arrl_sect"));
    record.setValue("my_cnty",contact.take("my_cnty"));
    record.setValue("my_cq_zone",contact.take("my_cq_zone"));
    record.setValue("my_dxcc",contact.take("my_dxcc"));
    record.setValue("my_fists",contact.take("my_fists"));
    record.setValue("my_gridsquare",contact.take("my_gridsquare").toString().toUpper());
    record.setValue("my_iota",contact.take("my_iota"));
    record.setValue("my_iota_island_id",contact.take("my_iota_island_id"));
    record.setValue("my_itu_zone",contact.take("my_itu_zone"));
    record.setValue("my_lat",contact.take("my_lat"));
    record.setValue("my_lon",contact.take("my_lon"));
    record.setValue("my_sota_ref",contact.take("my_sota_ref"));
    record.setValue("my_state",contact.take("my_state"));
    record.setValue("my_usaca_counties",contact.take("my_usaca_counties"));
    record.setValue("my_vucc_grids",contact.take("my_vucc_grids").toString().toUpper());
    record.setValue("my_wwff_ref",contact.take("my_wwff_ref").toString().toUpper());
    record.setValue("nr_bursts",contact.take("nr_bursts"));
    record.setValue("nr_pings",contact.take("nr_pings"));
    record.setValue("operator",contact.take("operator"));
    record.setValue("owner_callsign",contact.take("owner_callsign"));
    record.setValue("precedence",contact.take("precedence"));
    record.setValue("prop_mode",contact.take("prop_mode"));
    record.setValue("public_key",contact.take("public_key"));
    record.setValue("qrzcom_qso_upload_date",AdiFormat::parseDate(contact.take("qrzcom_qso_upload_date").toString()));
    record.setValue("qrzcom_qso_upload_status",AdiFormat::parseUploadStatus(contact.take("qrzcom_qso_upload_status").toString()));
    record.setValue("qsl_rcvd_via",contact.take("qsl_rcvd_via"));
    record.setValue("qsl_sent_via",contact.take("qsl_sent_via"));
    record.setValue("qsl_via",contact.take("qsl_via"));
    record.setValue("qso_complete",contact.take("qso_complete"));
    record.setValue("qso_random",contact.take("qso_random").toString());
    record.setValue("region",contact.take("region"));
    record.setValue("rx_pwr",contact.take("rx_pwr"));
    record.setValue("sat_mode",contact.take("sat_mode"));
    record.setValue("sat_name",contact.take("sat_name"));
    record.setValue("sfi",contact.take("sfi"));
    record.setValue("silent_key",contact.take("silent_key").toString().toUtf8());
    record.setValue("skcc",contact.take("skcc"));
    record.setValue("sota_ref",contact.take("sota_ref"));
    record.setValue("srx",contact.take("srx"));
    record.setValue("srx_string",contact.take("srx_string"));
    record.setValue("station_callsign",contact.take("station_callsign").toString().toUpper());
    record.setValue("stx",contact.take("stx"));
    record.setValue("stx_string",contact.take("stx_string"));
    record.setValue("swl",contact.take("swl").toString().toUpper());
    record.setValue("ten_ten",contact.take("ten_ten"));
    record.setValue("uksmg",contact.take("uksmg"));
    record.setValue("usaca_counties",contact.take("usaca_counties"));
    record.setValue("ve_prov",contact.take("ve_prov"));
    record.setValue("vucc_grids",contact.take("vucc_grids").toString().toUpper());
    record.setValue("web",contact.take("web"));
    record.setValue("wwff_ref",contact.take("wwff_ref").toString().toUpper());

    AdiFormat::importIntlField("name", "name_intl", record, contact);
    AdiFormat::importIntlField("address", "address_intl", record, contact);
    AdiFormat::importIntlField("comment", "comment_intl", record, contact);
    AdiFormat::importIntlField("country", "country_intl", record, contact);
    AdiFormat::importIntlField("my_antenna", "my_antenna_intl", record, contact);
    AdiFormat::importIntlField("my_city", "my_city_intl", record, contact);
    AdiFormat::importIntlField("my_country", "my_country_intl", record, contact);
    AdiFormat::importIntlField("my_name", "my_name_intl", record, contact);
    AdiFormat::importIntlField("my_postal_code", "my_postal_code_intl", record, contact);
    AdiFormat::importIntlField("my_rig", "my_rig_intl", record, contact);
    AdiFormat::importIntlField("my_sig", "my_sig_intl", record, contact);
    AdiFormat::importIntlField("my_sig_info", "my_sig_info_intl", record, contact);
    AdiFormat::importIntlField("my_street", "my_street_intl", record, contact);
    AdiFormat::importIntlField("notes", "notes_intl", record, contact);
    AdiFormat::importIntlField("qslmsg", "qslmsg_intl", record, contact);
    AdiFormat::importIntlField("qth", "qth_intl", record, contact);
    AdiFormat::importIntlField("rig", "rig_intl", record, contact);
    AdiFormat::importIntlField("sig", "sig_intl", record, contact);
    AdiFormat::importIntlField("sig_info", "sig_info_intl", record, contact);

    QString mode = contact.take("mode").toString().toUpper();
    QString submode = contact.take("submode").toString().toUpper();

    QPair<QString, QString> legacy = Data::instance()->legacyMode(mode);
    if (!legacy.first.isEmpty()) {
        mode = legacy.first;
        submode = legacy.second;
    }

    record.setValue("mode", mode);
    record.setValue("submode", submode);

    QDate date_on = AdiFormat::parseDate(contact.take("qso_date").toString());
    QDate date_off = AdiFormat::parseDate(contact.take("qso_date_off").toString());

    if (date_off.isNull() || !date_off.isValid()) {
        date_off = date_on;
    }

    QTime time_on = AdiFormat::parseTime(contact.take("time_on").toString());
    QTime time_off = AdiFormat::parseTime(contact.take("time_off").toString());

    if (time_on.isValid() && time_off.isNull()) {
        time_off = time_on;
    }
    if (time_off.isValid() && time_on.isNull()) {
        time_on = time_off;
    }

    QDateTime start_time(date_on, time_on, Qt::UTC);
    QDateTime end_time(date_off, time_off, Qt::UTC);

    if (end_time < start_time) {
        qCDebug(runtime) << "End time before start time!" << record;
    }

    record.setValue("start_time", start_time);
    record.setValue("end_time", end_time);

    /* If we have something unparsed then stored it as JSON to Field column */
    if ( contact.count() > 0 )
    {
        QJsonDocument doc = QJsonDocument::fromVariant(QVariant(contact));
        record.setValue("fields", QString(doc.toJson()));
    }

    return true;
}

void AdxFormat::exportContact(const QSqlRecord& record, QMap<QString, QString> *applTags) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<record;

    writer->writeStartElement("RECORD");

    QDateTime time_start = record.value("start_time").toDateTime().toTimeSpec(Qt::UTC);
    QDateTime time_end = record.value("end_time").toDateTime().toTimeSpec(Qt::UTC);

    writeField("call", record.value("callsign").toString());
    writeField("qso_date", time_start.toString("yyyyMMdd"));
    writeField("time_on", time_start.toString("hhmmss"));
    writeField("qso_date_off", time_end.toString("yyyyMMdd"));
    writeField("time_off", time_end.toString("hhmmss"));
    writeField("rst_rcvd", record.value("rst_rcvd").toString());
    writeField("rst_sent", record.value("rst_sent").toString());
    writeField("name", record.value("name").toString());
    writeField("qth", record.value("qth").toString());
    writeField("gridsquare", record.value("gridsquare").toString());
    writeField("cqz", record.value("cqz").toString());
    writeField("ituz", record.value("ituz").toString());
    writeField("freq", record.value("freq").toString());
    writeField("band", record.value("band").toString());
    writeField("mode", record.value("mode").toString());
    writeField("submode", record.value("submode").toString());
    writeField("cont", record.value("cont").toString());
    writeField("dxcc", record.value("dxcc").toString());
    writeField("country", record.value("country").toString());
    writeField("pfx", record.value("pfx").toString());
    writeField("state", record.value("state").toString());
    writeField("cnty", record.value("cnty").toString());
    writeField("iota", record.value("iota").toString());
    writeField("qsl_rcvd", record.value("qsl_rcvd").toString());
    writeField("qslrdate", record.value("qslrdate").toDate().toString("yyyyMMdd"));
    writeField("qsl_sent", record.value("qsl_sent").toString());
    writeField("qslsdate", record.value("qslsdate").toDate().toString("yyyyMMdd"));
    writeField("lotw_qsl_rcvd", record.value("lotw_qsl_rcvd").toString());
    writeField("lotw_qslrdate", record.value("lotw_qslrdate").toDate().toString("yyyyMMdd"));
    writeField("lotw_qsl_sent", record.value("lotw_qsl_sent").toString());
    writeField("lotw_qslsdate", record.value("lotw_qslsdate").toDate().toString("yyyyMMdd"));
    writeField("tx_pwr", record.value("tx_pwr").toString());
    writeField("address", record.value("address").toString());
    writeField("address_intl", record.value("address").toString());
    writeField("age", record.value("age").toString());
    writeField("a_index", record.value("a_index").toString());
    writeField("ant_az", record.value("ant_az").toString());
    writeField("ant_el", record.value("ant_el").toString());
    writeField("ant_path", record.value("ant_path").toString());
    writeField("arrl_sect", record.value("arrl_sect").toString());
    writeField("award_submitted", record.value("award_submitted").toString());
    writeField("award_granted", record.value("award_granted").toString());
    writeField("band_rx", record.value("band_rx").toString());
    writeField("check", record.value("check").toString());
    writeField("class", record.value("class").toString());
    writeField("clublog_qso_upload_date", record.value("clublog_qso_upload_date").toDate().toString("yyyyMMdd"));
    writeField("clublog_qso_upload_status", record.value("clublog_qso_upload_status").toString());
    writeField("comment", record.value("comment").toString());
    writeField("comment_intl", record.value("comment_intl").toString());
    writeField("contacted_op", record.value("contacted_op").toString());
    writeField("contest_id", record.value("contest_id").toString());
    writeField("country_intl", record.value("country_intl").toString());
    writeField("credit_submitted", record.value("credit_submitted").toString());
    writeField("credit_granted", record.value("credit_granted").toString());
    writeField("darc_dok", record.value("darc_dok").toString());
    writeField("distance", record.value("distance").toString());
    writeField("email", record.value("email").toString());
    writeField("eq_call", record.value("eq_call").toString());
    writeField("eqsl_qslrdate", record.value("eqsl_qslrdate").toDate().toString("yyyyMMdd"));
    writeField("eqsl_qslsdate", record.value("eqsl_qslsdate").toDate().toString("yyyyMMdd"));
    writeField("eqsl_qsl_rcvd", record.value("eqsl_qsl_rcvd").toString());
    writeField("eqsl_qsl_sent", record.value("eqsl_qsl_sent").toString());
    writeField("fists", record.value("fists").toString());
    writeField("fists_cc", record.value("fists_cc").toString());
    writeField("force_init", record.value("force_init").toString());
    writeField("freq_rx", record.value("freq_rx").toString());
    writeField("guest_op", record.value("guest_op").toString());
    writeField("hrdlog_qso_upload_date", record.value("hrdlog_qso_upload_date").toDate().toString("yyyyMMdd"));
    writeField("hrdlog_qso_upload_status", record.value("hrdlog_qso_upload_status").toString());
    writeField("iota_island_id", record.value("iota_island_id").toString());
    writeField("k_index", record.value("k_index").toString());
    writeField("lat", record.value("lat").toString());
    writeField("lon", record.value("lon").toString());
    writeField("max_bursts", record.value("max_bursts").toString());
    writeField("ms_shower", record.value("ms_shower").toString());
    writeField("my_arrl_sect", record.value("my_arrl_sect").toString());
    writeField("my_antenna", record.value("my_antenna").toString());
    writeField("my_antenna_intl", record.value("my_antenna_intl").toString());
    writeField("my_city", record.value("my_city").toString());
    writeField("my_city_intl", record.value("my_city_intl").toString());
    writeField("my_cnty", record.value("my_cnty").toString());
    writeField("my_country", record.value("my_country").toString());
    writeField("my_country_intl", record.value("my_country_intl").toString());
    writeField("my_cq_zone", record.value("my_cq_zone").toString());
    writeField("my_dxcc", record.value("my_dxcc").toString());
    writeField("my_fists", record.value("my_fists").toString());
    writeField("my_gridsquare", record.value("my_gridsquare").toString());
    writeField("my_iota", record.value("my_iota").toString());
    writeField("my_iota_island_id", record.value("my_iota_island_id").toString());
    writeField("my_itu_zone", record.value("my_itu_zone").toString());
    writeField("my_lat", record.value("my_lat").toString());
    writeField("my_lon", record.value("my_lon").toString());
    writeField("my_name", record.value("my_name").toString());
    writeField("my_name_intl", record.value("my_name_intl").toString());
    writeField("my_postal_code", record.value("my_postal_code").toString());
    writeField("my_postal_code_intl", record.value("my_postal_code_intl").toString());
    writeField("my_rig", record.value("my_rig").toString());
    writeField("my_rig_intl", record.value("my_rig_intl").toString());
    writeField("my_sig", record.value("my_sig").toString());
    writeField("my_sig_intl", record.value("my_sig_intl").toString());
    writeField("my_sig_info", record.value("my_sig_info").toString());
    writeField("my_sig_info_intl", record.value("my_sig_info_intl").toString());
    writeField("my_sota_ref", record.value("my_sota_ref").toString());
    writeField("my_state", record.value("my_state").toString());
    writeField("my_street", record.value("my_street").toString());
    writeField("my_street_intl", record.value("my_street_intl").toString());
    writeField("my_usaca_counties", record.value("my_usaca_counties").toString());
    writeField("my_vucc_grids", record.value("my_vucc_grids").toString());
    writeField("my_wwff_ref", record.value("my_wwff_ref").toString());
    writeField("name_intl", record.value("name_intl").toString());
    writeField("notes", record.value("notes").toString());
    writeField("notes_intl", record.value("notes_intl").toString());
    writeField("nr_bursts", record.value("nr_bursts").toString());
    writeField("nr_pings", record.value("nr_pings").toString());
    writeField("operator", record.value("operator").toString());
    writeField("owner_callsign", record.value("owner_callsign").toString());
    writeField("precedence", record.value("precedence").toString());
    writeField("prop_mode", record.value("prop_mode").toString());
    writeField("public_key", record.value("public_key").toString());
    writeField("qrzcom_qso_upload_date", record.value("qrzcom_qso_upload_date").toDate().toString("yyyyMMdd"));
    writeField("qrzcom_qso_upload_status", record.value("qrzcom_qso_upload_status").toString());
    writeField("qslmsg", record.value("qslmsg").toString());
    writeField("qslmsg_intl", record.value("qslmsg_intl").toString());
    writeField("qsl_rcvd_via", record.value("qsl_rcvd_via").toString());
    writeField("qsl_sent_via", record.value("qsl_sent_via").toString());
    writeField("qsl_via", record.value("qsl_via").toString());
    writeField("qso_complete", record.value("qso_complete").toString());
    writeField("qso_random", record.value("qso_random").toString());
    writeField("qth_intl", record.value("qth_intl").toString());
    writeField("region", record.value("region").toString());
    writeField("rig", record.value("rig").toString());
    writeField("rig_intl", record.value("rig_intl").toString());
    writeField("rx_pwr", record.value("rx_pwr").toString());
    writeField("sat_mode", record.value("sat_mode").toString());
    writeField("sat_name", record.value("sat_name").toString());
    writeField("sfi", record.value("sfi").toString());
    writeField("sig", record.value("sig").toString());
    writeField("sig_intl", record.value("sig_intl").toString());
    writeField("sig_info", record.value("sig_info").toString());
    writeField("sig_info_intl", record.value("sig_info_intl").toString());
    writeField("silent_key", record.value("silent_key").toString());
    writeField("skcc", record.value("skcc").toString());
    writeField("sota_ref", record.value("sota_ref").toString());
    writeField("srx", record.value("srx").toString());
    writeField("srx_string", record.value("srx_string").toString());
    writeField("station_callsign", record.value("station_callsign").toString());
    writeField("stx", record.value("stx").toString());
    writeField("stx_string", record.value("stx_string").toString());
    writeField("swl", record.value("swl").toString());
    writeField("ten_ten", record.value("ten_ten").toString());
    writeField("uksmg", record.value("uksmg").toString());
    writeField("usaca_counties", record.value("usaca_counties").toString());
    writeField("ve_prov", record.value("ve_prov").toString());
    writeField("vucc_grids", record.value("vucc_grids").toString());
    writeField("web", record.value("web").toString());
    writeField("wwff_ref", record.value("wwff_ref").toString());

    QJsonObject fields = QJsonDocument::fromJson(record.value("fields").toByteArray()).object();

    auto keys = fields.keys();
    for (const QString& key : qAsConst(keys))
    {
        writeField(key, fields.value(key).toString());
    }

    /* Add application-specific tags */
    if ( applTags )
    {
       auto keys = applTags->keys();
       for (auto &key : qAsConst(keys))
       {
           writeField(key, applTags->value(key));
       }
    }

    writer->writeEndElement();
}

void AdxFormat::writeField(QString name, QString value) {
    FCT_IDENTIFICATION;

    qCDebug(function_parameters)<<name << " " << value;

    if (value.isEmpty()) return;
    writer->writeTextElement(name.toUpper(), value);
}

bool AdxFormat::readContact(QVariantMap & contact)
{
    FCT_IDENTIFICATION;

    while ( !reader->atEnd() )
    {
        reader->readNextStartElement();

        qCDebug(runtime)<<reader->name();

        if ( reader->name() == "RECORDS" && reader->isEndElement() )
        {
            qCDebug(runtime)<<"End Records Element";
            return false;
        }
        if ( reader->name() == "RECORD" )
        {
            while (reader->readNextStartElement() )
            {
                qCDebug(runtime)<<"adding element " << reader->name();
                contact[reader->name().toLatin1().toLower()] = QVariant(reader->readElementText());
            }
            return true;
        }
        else
        {
            reader->skipCurrentElement();
        }
    }
    return false;
}
