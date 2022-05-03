CREATE TABLE IF NOT EXISTS alert_rules
(
        rule_name text PRIMARY KEY,
        enabled INTEGER,
        source INTEGER,
        dx_callsign TEXT,
        dx_country INTEGER,
        dx_logstatus INTEGER,
        dx_continent TEXT,
        spot_comment TEXT,
        mode TEXT,
        band TEXT,
        spotter_country INTEGER,
        spotter_continent TEXT
);
