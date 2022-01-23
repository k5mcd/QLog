#include <QSqlRecord>
#include <QtXml>
#include "logformat/AdxFormat.h"
#include "core/debug.h"

MODULE_IDENTIFICATION("qlog.logformat.adxformat");

void AdxFormat::exportStart() {
    FCT_IDENTIFICATION;

    QString date = QDateTime::currentDateTimeUtc().toString("yyyyMMdd hhmmss");

    writer = new QXmlStreamWriter(stream.device());
    writer->setAutoFormatting(true);

    writer->writeStartDocument();
    writer->writeStartElement("ADX");

    writer->writeStartElement("HEADER");
    writer->writeTextElement("ADIF_VER", "3.1.2");
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
