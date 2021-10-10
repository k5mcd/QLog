ALTER TABLE contacts  ADD address TEXT;
ALTER TABLE contacts  ADD address_intl TEXT;
ALTER TABLE contacts  ADD age INTEGER;
ALTER TABLE contacts  ADD a_index INTEGER;
ALTER TABLE contacts  ADD ant_az INTEGER;
ALTER TABLE contacts  ADD ant_el INTEGER;
ALTER TABLE contacts  ADD ant_path CHECK(ant_path IN ('G', 'O', 'S', 'L'));
ALTER TABLE contacts  ADD arrl_sect TEXT;
ALTER TABLE contacts ADD award_submitted TEXT;
ALTER TABLE contacts ADD award_granted TEXT;
ALTER TABLE contacts ADD band_rx TEXT;
ALTER TABLE contacts ADD "check" TEXT;
ALTER TABLE contacts ADD "class" TEXT;
ALTER TABLE contacts ADD clublog_qso_upload_date TEXT;
ALTER TABLE contacts ADD clublog_qso_upload_status CHECK(clublog_qso_upload_status IN ('N', 'Y', 'M')) DEFAULT 'N';
ALTER TABLE contacts ADD "comment" TEXT;
ALTER TABLE contacts ADD comment_intl TEXT;
ALTER TABLE contacts ADD contacted_op TEXT;
ALTER TABLE contacts ADD contest_id TEXT;
ALTER TABLE contacts ADD country_intl TEXT;
ALTER TABLE contacts ADD credit_submitted TEXT;
ALTER TABLE contacts ADD credit_granted TEXT;
ALTER TABLE contacts ADD darc_dok TEXT;
ALTER TABLE contacts ADD distance REAL;
ALTER TABLE contacts ADD email TEXT;
ALTER TABLE contacts ADD eq_call TEXT;
ALTER TABLE contacts ADD eqsl_qslrdate TEXT;
ALTER TABLE contacts ADD eqsl_qslsdate TEXT;
ALTER TABLE contacts ADD eqsl_qsl_rcvd CHECK(eqsl_qsl_rcvd IN ('N', 'Y', 'R', 'I')) DEFAULT 'N';
ALTER TABLE contacts ADD eqsl_qsl_sent CHECK(eqsl_qsl_sent IN ('N', 'Y', 'R', 'I')) DEFAULT 'N';
ALTER TABLE contacts ADD fists INTEGER;
ALTER TABLE contacts ADD fists_cc INTEGER;
ALTER TABLE contacts ADD force_init CHECK(force_init IN ('N', 'Y'));
ALTER TABLE contacts ADD freq_rx REAL;
ALTER TABLE contacts ADD guest_op TEXT;
ALTER TABLE contacts ADD hrdlog_qso_upload_date TEXT;
ALTER TABLE contacts ADD hrdlog_qso_upload_status CHECK(hrdlog_qso_upload_status IN ('N', 'Y', 'R', 'I')) DEFAULT 'N';
ALTER TABLE contacts ADD iota_island_id INTEGER;
ALTER TABLE contacts ADD k_index INTEGER;
ALTER TABLE contacts ADD lat TEXT;
ALTER TABLE contacts ADD lon TEXT;
ALTER TABLE contacts ADD max_bursts REAL;
ALTER TABLE contacts ADD ms_shower TEXT;
ALTER TABLE contacts ADD my_antenna TEXT;
ALTER TABLE contacts ADD my_antenna_intl TEXT;
ALTER TABLE contacts ADD my_city TEXT;
ALTER TABLE contacts ADD my_city_intl TEXT;
ALTER TABLE contacts ADD my_cnty TEXT;
ALTER TABLE contacts ADD my_country TEXT;
ALTER TABLE contacts ADD my_country_intl TEXT;
ALTER TABLE contacts ADD my_cq_zone INTEGER;
ALTER TABLE contacts ADD my_dxcc INTEGER;
ALTER TABLE contacts ADD my_fists INTEGER;
ALTER TABLE contacts ADD my_gridsquare TEXT;
ALTER TABLE contacts ADD my_iota TEXT;
ALTER TABLE contacts ADD my_iota_island_id INTEGER;
ALTER TABLE contacts ADD my_itu_zone INTEGER;
ALTER TABLE contacts ADD my_lat TEXT;
ALTER TABLE contacts ADD my_lon TEXT;
ALTER TABLE contacts ADD my_name TEXT;
ALTER TABLE contacts ADD my_name_intl TEXT;
ALTER TABLE contacts ADD my_postal_code TEXT;
ALTER TABLE contacts ADD my_postal_code_intl TEXT;
ALTER TABLE contacts ADD my_rig TEXT;
ALTER TABLE contacts ADD my_rig_intl TEXT;
ALTER TABLE contacts ADD my_sig TEXT;
ALTER TABLE contacts ADD my_sig_intl TEXT;
ALTER TABLE contacts ADD my_sig_info TEXT;
ALTER TABLE contacts ADD my_sig_info_intl TEXT;
ALTER TABLE contacts ADD my_sota_ref TEXT;
ALTER TABLE contacts ADD my_state TEXT;
ALTER TABLE contacts ADD my_street TEXT;
ALTER TABLE contacts ADD my_street_intl TEXT;
ALTER TABLE contacts ADD my_usaca_counties TEXT;
ALTER TABLE contacts ADD my_vucc_grids TEXT;
ALTER TABLE contacts ADD name_intl TEXT;
ALTER TABLE contacts ADD notes TEXT;
ALTER TABLE contacts ADD notes_intl TEXT;
ALTER TABLE contacts ADD nr_bursts INTEGER;
ALTER TABLE contacts ADD nr_pings INTEGER;
ALTER TABLE contacts ADD "operator" TEXT;
ALTER TABLE contacts ADD owner_callsign TEXT;
ALTER TABLE contacts ADD precedence TEXT;
ALTER TABLE contacts ADD prop_mode TEXT;
ALTER TABLE contacts ADD public_key TEXT;
ALTER TABLE contacts ADD qrzcom_qso_upload_date TEXT;
ALTER TABLE contacts ADD qrzcom_qso_upload_status CHECK(qrzcom_qso_upload_status IN ('N', 'Y', 'M')) DEFAULT 'N';
ALTER TABLE contacts ADD qslmsg TEXT;
ALTER TABLE contacts ADD qslmsg_intl TEXT;
ALTER TABLE contacts ADD qsl_rcvd_via CHECK(qsl_rcvd_via IN ('B', 'D', 'E', 'M'));
ALTER TABLE contacts ADD qsl_sent_via CHECK(qsl_sent_via IN ('B', 'D', 'E', 'M'));
ALTER TABLE contacts ADD qsl_via TEXT;
ALTER TABLE contacts ADD qso_complete CHECK(qso_complete IN ('Y', 'N', 'NIL', '?' ));
ALTER TABLE contacts ADD qso_random CHECK(qso_random IN ('N', 'Y'));
ALTER TABLE contacts ADD qth_intl TEXT;
ALTER TABLE contacts ADD region TEXT;
ALTER TABLE contacts ADD rig TEXT;
ALTER TABLE contacts ADD rig_intl TEXT;
ALTER TABLE contacts ADD rx_pwr REAL;
ALTER TABLE contacts ADD sat_mode CHECK(sat_mode IN ('K', 'T', 'A', 'A', 'J', 'B', 'S', 'L', 'VU', 'UV', 'US', 'LU', 'LS', 'LX', 'VS'));
ALTER TABLE contacts ADD sat_name TEXT;
ALTER TABLE contacts ADD sfi INTEGER;
ALTER TABLE contacts ADD sig TEXT;
ALTER TABLE contacts ADD sig_intl TEXT;
ALTER TABLE contacts ADD sig_info TEXT;
ALTER TABLE contacts ADD sig_info_intl TEXT;
ALTER TABLE contacts ADD silent_key CHECK(silent_key IN ('N', 'Y'));
ALTER TABLE contacts ADD skcc TEXT;
ALTER TABLE contacts ADD sota_ref TEXT;
ALTER TABLE contacts ADD srx INTEGER;
ALTER TABLE contacts ADD srx_string TEXT;
ALTER TABLE contacts ADD station_callsign TEXT;
ALTER TABLE contacts ADD stx INTEGER;
ALTER TABLE contacts ADD stx_string TEXT;
ALTER TABLE contacts ADD swl CHECK(swl IN ('N', 'Y'));
ALTER TABLE contacts ADD ten_ten INTEGER;
ALTER TABLE contacts ADD uksmg INTEGER;
ALTER TABLE contacts ADD usaca_counties TEXT;
ALTER TABLE contacts ADD ve_prov TEXT;
ALTER TABLE contacts ADD vucc_grids TEXT;
ALTER TABLE contacts ADD web TEXT;

CREATE TABLE IF NOT EXISTS sat_info (
        name TEXT NOT NULL,
        number INTEGER,
        uplink TEXT,
        downlink TEXT,
        beacon TEXT,
        "mode" TEXT,
        callsign TEXT,
        status TEXT
);

CREATE TABLE IF NOT EXISTS qso_filter_matching_types (
        matching_id INTEGER PRIMARY KEY,
        sql_operator TEXT NOT NULL
);

INSERT INTO qso_filter_matching_types values (0,'AND');
INSERT INTO qso_filter_matching_types values (1,'OR');

CREATE TABLE IF NOT EXISTS qso_filter_operators (
        operator_id INTEGER PRIMARY KEY,
        sql_operator TEXT NOT NULL
);

INSERT INTO qso_filter_operators values (0,'=');
INSERT INTO qso_filter_operators values (1,'<>');
INSERT INTO qso_filter_operators values (2,'like');
INSERT INTO qso_filter_operators values (3,'not like');
INSERT INTO qso_filter_operators values (4,'>');
INSERT INTO qso_filter_operators values (5,'<');

CREATE TABLE IF NOT EXISTS qso_filters (
        filter_name text PRIMARY KEY,
        matching_type INTEGER REFERENCES qso_filter_matching_types(matching_id)
);

CREATE TABLE IF NOT EXISTS qso_filter_rules (
        filter_name       TEXT REFERENCES qso_filters(filter_name) ON DELETE CASCADE,
        table_field_index INTEGER NOT NULL,
        operator_id       INTEGER REFERENCES qso_filter_operators(operator_id) ,
        "value"           TEXT
);
