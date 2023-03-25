CREATE TABLE IF NOT EXISTS rot_user_buttons_profiles(
        profile_name TEXT PRIMARY KEY,
        button1_short  TEXT,
        button1_value  REAL DEFAULT '-1',
        button2_short  TEXT,
        button2_value  REAL DEFAULT '-1',
        button3_short  TEXT,
        button3_value  REAL DEFAULT '-1',
        button4_short  TEXT,
        button4_value  REAL DEFAULT '-1'
);

INSERT INTO rot_user_buttons_profiles(
profile_name,
button1_short, button1_value,
button2_short, button2_value,
button3_short, button3_value,
button4_short, button4_value
)
VALUES (
"Basic",
"N", 0,
"S", 180,
"W", 270,
"E", 90
);

UPDATE contacts SET ant_az = 360 + ant_az WHERE ant_az < 0;

CREATE TABLE IF NOT EXISTS membership_directory(
        short_desc  TEXT,
        long_desc   TEXT,
        filename    TEXT,
        last_update TEXT,
        num_records INTEGER
);

CREATE TABLE IF NOT EXISTS membership (
        callsign     TEXT,
        member_id    TEXT,
        valid_from   TEXT,
        valid_to     TEXT,
        clubid       TEXT
);

CREATE TABLE IF NOT EXISTS membership_versions (
        clubid     TEXT PRIMARY KEY,
        version    INTEGER
);

CREATE INDEX IF NOT EXISTS membership_callsign_idx ON membership(callsign);
CREATE INDEX IF NOT EXISTS membership_clubid_idx ON membership(clubid);

CREATE TABLE IF NOT EXISTS contacts_autovalue(
   contactid INTEGER PRIMARY KEY REFERENCES contacts("id") ON DELETE CASCADE,
   base_callsign TEXT
);

CREATE INDEX IF NOT EXISTS contacts_autovalue_call_idx on contacts_autovalue(base_callsign);

CREATE VIEW IF NOT EXISTS contact_clubs_view AS
SELECT contactid, clubid
FROM contacts_autovalue c, membership m
WHERE c.base_callsign = m.callsign;


INSERT INTO contacts_autovalue SELECT ID, ((WITH tokenizedCallsign(word, csv) AS ( SELECT '', callsign||'/'
                                                                             UNION ALL
                                                                             SELECT substr(csv, 0, instr(csv, '/')), substr(csv, instr(csv, '/') + 1)
                                                                             FROM tokenizedCallsign
                                                                             WHERE csv != '' )
                                           SELECT word FROM tokenizedCallsign
                                           WHERE word != ''
                                           AND word REGEXP '^([A-Z][0-9]|[A-Z]{1,2}|[0-9][A-Z])([0-9]|[0-9]+)([A-Z]+)$' LIMIT 1)
                                           ) FROM contacts;

ALTER TABLE alert_rules ADD dx_member TEXT DEFAULT '*';
