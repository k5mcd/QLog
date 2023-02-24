ALTER TABLE contacts RENAME TO contacts_old;

DROP INDEX callsign_idx;
DROP INDEX dxcc_idx;

CREATE TABLE IF NOT EXISTS adif_enum_qsl_rcvd (
        "value"    TEXT PRIMARY KEY);
INSERT INTO adif_enum_qsl_rcvd VALUES ('Y');
INSERT INTO adif_enum_qsl_rcvd VALUES ('N');
INSERT INTO adif_enum_qsl_rcvd VALUES ('R');
INSERT INTO adif_enum_qsl_rcvd VALUES ('I');
INSERT INTO adif_enum_qsl_rcvd VALUES ('V');

CREATE TABLE IF NOT EXISTS adif_enum_qsl_sent (
        "value"    TEXT PRIMARY KEY);
INSERT INTO adif_enum_qsl_sent VALUES ('Y');
INSERT INTO adif_enum_qsl_sent VALUES ('N');
INSERT INTO adif_enum_qsl_sent VALUES ('R');
INSERT INTO adif_enum_qsl_sent VALUES ('Q');
INSERT INTO adif_enum_qsl_sent VALUES ('I');

CREATE TABLE IF NOT EXISTS adif_enum_ant_path (
        "value"    TEXT PRIMARY KEY);
INSERT INTO adif_enum_ant_path VALUES ('G');
INSERT INTO adif_enum_ant_path VALUES ('O');
INSERT INTO adif_enum_ant_path VALUES ('S');
INSERT INTO adif_enum_ant_path VALUES ('L');

CREATE TABLE IF NOT EXISTS adif_enum_qso_upload_status (
        "value"    TEXT PRIMARY KEY);
INSERT INTO adif_enum_qso_upload_status VALUES ('Y');
INSERT INTO adif_enum_qso_upload_status VALUES ('N');
INSERT INTO adif_enum_qso_upload_status VALUES ('M');

CREATE TABLE IF NOT EXISTS adif_enum_qsl_via (
        "value"    TEXT PRIMARY KEY);
INSERT INTO adif_enum_qsl_via VALUES ('B');
INSERT INTO adif_enum_qsl_via VALUES ('D');
INSERT INTO adif_enum_qsl_via VALUES ('E');
INSERT INTO adif_enum_qsl_via VALUES ('M');

CREATE TABLE IF NOT EXISTS adif_enum_qso_complete (
        "value"    TEXT PRIMARY KEY);
INSERT INTO adif_enum_qso_complete VALUES ('Y');
INSERT INTO adif_enum_qso_complete VALUES ('N');
INSERT INTO adif_enum_qso_complete VALUES ('NIL');
INSERT INTO adif_enum_qso_complete VALUES ('?');

CREATE TABLE IF NOT EXISTS adif_enum_boolean (
        "value"    TEXT PRIMARY KEY);
INSERT INTO adif_enum_boolean VALUES ('Y');
INSERT INTO adif_enum_boolean VALUES ('N');

CREATE TABLE "contacts" (
 "id"                            INTEGER,
 "start_time"                    TEXT,
 "end_time"                      TEXT,
 "callsign"                      TEXT NOT NULL,
 "rst_sent"                      TEXT,
 "rst_rcvd"                      TEXT,
 "freq"                          REAL,
 "band"                          TEXT,
 "mode"                          TEXT,
 "submode"                       TEXT,
 "name"                          TEXT,
 "qth"                           TEXT,
 "gridsquare"                    TEXT,
 "dxcc"                          INTEGER,
 "country"                       TEXT,
 "cont"                          TEXT,
 "cqz"                           INTEGER,
 "ituz"                          INTEGER,
 "pfx"                           TEXT,
 "state"                         TEXT,
 "cnty"                          TEXT,
 "iota"                          TEXT,
 "qsl_rcvd"                      TEXT NOT NULL DEFAULT 'N' REFERENCES adif_enum_qsl_rcvd("value"),
 "qsl_rdate"                     TEXT,
 "qsl_sent"                      TEXT NOT NULL DEFAULT 'N' REFERENCES adif_enum_qsl_sent("value"),
 "qsl_sdate"                     TEXT,
 "lotw_qsl_rcvd"                 TEXT NOT NULL DEFAULT 'N' REFERENCES adif_enum_qsl_rcvd("value"),
 "lotw_qslrdate"                 TEXT,
 "lotw_qsl_sent"                 TEXT NOT NULL DEFAULT 'N' REFERENCES adif_enum_qsl_sent("value"),
 "lotw_qslsdate"                 TEXT,
 "tx_pwr"                        REAL,
 "fields"                        JSON,
 "address"                       TEXT,
 "address_intl"                  TEXT,
 "age"                           INTEGER,
 "a_index"                       INTEGER,
 "ant_az"                        INTEGER,
 "ant_el"                        INTEGER,
 "ant_path"                      TEXT REFERENCES adif_enum_ant_path("value"),
 "arrl_sect"                     TEXT,
 "award_submitted"               TEXT,
 "award_granted"                 TEXT,
 "band_rx"                       TEXT,
 "check"                         TEXT,
 "class"                         TEXT,
 "clublog_qso_upload_date"       TEXT,
 "clublog_qso_upload_status"     TEXT REFERENCES adif_enum_qso_upload_status("value"),
 "comment"                       TEXT,
 "comment_intl"                  TEXT,
 "contacted_op"                  TEXT,
 "contest_id"                    TEXT,
 "country_intl"                  TEXT,
 "credit_submitted"              TEXT,
 "credit_granted"                TEXT,
 "darc_dok"                      TEXT,
 "distance"                      REAL,
 "email"                         TEXT,
 "eq_call"                       TEXT,
 "eqsl_qslrdate"                 TEXT,
 "eqsl_qslsdate"                 TEXT,
 "eqsl_qsl_rcvd"                 TEXT NOT NULL DEFAULT 'N' REFERENCES adif_enum_qsl_rcvd("value"),
 "eqsl_qsl_sent"                 TEXT NOT NULL DEFAULT 'N' REFERENCES adif_enum_qsl_sent("value"),
 "fists"                         INTEGER,
 "fists_cc"                      INTEGER,
 "force_init"                    TEXT REFERENCES adif_enum_boolean("value"),
 "freq_rx"                       REAL,
 "guest_op"                      TEXT,
 "hrdlog_qso_upload_date"        TEXT,
 "hrdlog_qso_upload_status"      TEXT REFERENCES adif_enum_qso_upload_status("value"),
 "iota_island_id"                INTEGER,
 "k_index"                       INTEGER,
 "lat"                           TEXT,
 "lon"                           TEXT,
 "max_bursts"                    REAL,
 "ms_shower"                     TEXT,
 "my_antenna"                    TEXT,
 "my_antenna_intl"               TEXT,
 "my_city"                       TEXT,
 "my_city_intl"                  TEXT,
 "my_cnty"                       TEXT,
 "my_country"                    TEXT,
 "my_country_intl"               TEXT,
 "my_cq_zone"                    INTEGER,
 "my_dxcc"                       INTEGER,
 "my_fists"                      INTEGER,
 "my_gridsquare"                 TEXT,
 "my_iota"                       TEXT,
 "my_iota_island_id"             INTEGER,
 "my_itu_zone"                   INTEGER,
 "my_lat"                        TEXT,
 "my_lon"                        TEXT,
 "my_name"                       TEXT,
 "my_name_intl"                  TEXT,
 "my_postal_code"                TEXT,
 "my_postal_code_intl"           TEXT,
 "my_rig"                        TEXT,
 "my_rig_intl"                   TEXT,
 "my_sig"                        TEXT,
 "my_sig_intl"                   TEXT,
 "my_sig_info"                   TEXT,
 "my_sig_info_intl"              TEXT,
 "my_sota_ref"                   TEXT,
 "my_state"                      TEXT,
 "my_street"                     TEXT,
 "my_street_intl"                TEXT,
 "my_usaca_counties"             TEXT,
 "my_vucc_grids"                 TEXT,
 "name_intl"                     TEXT,
 "notes"                         TEXT,
 "notes_intl"                    TEXT,
 "nr_bursts"                     INTEGER,
 "nr_pings"                      INTEGER,
 "operator"                      TEXT,
 "owner_callsign"                TEXT,
 "precedence"                    TEXT,
 "prop_mode"                     TEXT,
 "public_key"                    TEXT,
 "qrzcom_qso_upload_date"        TEXT,
 "qrzcom_qso_upload_status"      TEXT REFERENCES adif_enum_qso_upload_status("value"),
 "qslmsg"                        TEXT,
 "qslmsg_intl"                   TEXT,
 "qsl_rcvd_via"                  TEXT REFERENCES adif_enum_qsl_via("value"),
 "qsl_sent_via"                  TEXT REFERENCES adif_enum_qsl_via("value"),
 "qsl_via"                       TEXT,
 "qso_complete"                  TEXT REFERENCES adif_enum_qso_complete("value"),
 "qso_random"                    TEXT REFERENCES adif_enum_boolean("value"),
 "qth_intl"                      TEXT,
 "region"                        TEXT,
 "rig"                           TEXT,
 "rig_intl"                      TEXT,
 "rx_pwr"                        REAL,
 "sat_mode"                      TEXT,
 "sat_name"                      TEXT,
 "sfi"                           INTEGER,
 "sig"                           TEXT,
 "sig_intl"                      TEXT,
 "sig_info"                      TEXT,
 "sig_info_intl"                 TEXT,
 "silent_key"                    TEXT REFERENCES adif_enum_boolean("value"),
 "skcc"                          TEXT,
 "sota_ref"                      TEXT,
 "srx"                           INTEGER,
 "srx_string"                    TEXT,
 "station_callsign"              TEXT,
 "stx"                           INTEGER,
 "stx_string"                    TEXT,
 "swl"                           TEXT REFERENCES adif_enum_boolean("value"),
 "ten_ten"                       INTEGER,
 "uksmg"                         INTEGER,
 "usaca_counties"                TEXT,
 "ve_prov"                       TEXT,
 "vucc_grids"                    TEXT,
 "web"                           TEXT,
 "my_arrl_sect"                  TEXT,
 "my_wwff_ref"                   TEXT,
 "wwff_ref"                      TEXT,
 "altitude"                      REAL,
 "gridsquare_ext"                TEXT,
 "hamlogeu_qso_upload_date"      TEXT,
 "hamlogeu_qso_upload_status"    TEXT REFERENCES adif_enum_qso_upload_status("value"),
 "hamqth_qso_upload_date"        TEXT,
 "hamqth_qso_upload_status"      TEXT REFERENCES adif_enum_qso_upload_status("value"),
 "my_altitude"                   REAL,
 "my_gridsquare_ext"             TEXT,
 "my_pota_ref"                   TEXT,
 "pota_ref"                      TEXT,
 PRIMARY KEY("id")
);

CREATE INDEX "callsign_idx" ON "contacts" ("callsign");
CREATE INDEX "dxcc_idx" ON "contacts" ("dxcc");

INSERT INTO contacts SELECT * FROM contacts_old;

UPDATE contacts SET lotw_qsl_sent = 'Q' WHERE lotw_qsl_sent = 'N';
UPDATE contacts SET eqsl_qsl_sent = 'Q' WHERE eqsl_qsl_sent = 'N';
UPDATE contacts SET clublog_qso_upload_status = NULL WHERE clublog_qso_upload_status = 'N';
UPDATE contacts SET hrdlog_qso_upload_status = NULL WHERE hrdlog_qso_upload_status = 'N';
UPDATE contacts SET qrzcom_qso_upload_status = NULL WHERE qrzcom_qso_upload_status = 'N';
UPDATE contacts SET hamlogeu_qso_upload_status = NULL WHERE hamlogeu_qso_upload_status = 'N';
UPDATE contacts SET hamqth_qso_upload_status = NULL WHERE hamqth_qso_upload_status = 'N';
