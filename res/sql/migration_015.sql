UPDATE modes SET submodes = '["C4FM", "DMR", "DSTAR", "FREEDV", "M17" ]' WHERE name = 'DIGITALVOICE';

INSERT INTO bands (name, start_freq, end_freq, enabled) VALUES
('submm',300000.0, 7500000.0, 0);

ALTER TABLE contacts ADD altitude REAL;
ALTER TABLE contacts ADD gridsquare_ext TEXT;
ALTER TABLE contacts ADD hamlogeu_qso_upload_date TEXT;
ALTER TABLE contacts ADD hamlogeu_qso_upload_status CHECK(clublog_qso_upload_status IN ('N', 'Y', 'M')) DEFAULT 'N';
ALTER TABLE contacts ADD hamqth_qso_upload_date TEXT;
ALTER TABLE contacts ADD hamqth_qso_upload_status CHECK(clublog_qso_upload_status IN ('N', 'Y', 'M')) DEFAULT 'N';
ALTER TABLE contacts ADD my_altitude REAL;
ALTER TABLE contacts ADD my_gridsquare_ext TEXT;
ALTER TABLE contacts ADD my_pota_ref TEXT;
ALTER TABLE contacts ADD pota_ref TEXT;

CREATE TABLE IF NOT EXISTS pota_directory(
        reference TEXT PRIMARY KEY,
        name TEXT,
        active INTEGER,
        entityID INTEGER,
        locationDesc TEXT,
        latitude REAL,
        longitude REAL,
        grid TEXT
);

ALTER TABLE station_profiles ADD pota TEXT;
