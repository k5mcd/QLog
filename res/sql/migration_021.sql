CREATE TABLE IF NOT EXISTS chat_highlight_rules(
   rule_name TEXT PRIMARY KEY,
   room_id TEXT,
   enabled INTEGER,
   rule_definition TEXT
);

ALTER TABLE ant_profiles ADD azimuth_beamwidth DOUBLE DEFAULT 0.0;
ALTER TABLE ant_profiles ADD azimuth_offset DOUBLE DEFAULT 0.0;
