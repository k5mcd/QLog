CREATE TABLE IF NOT EXISTS station_profiles (
        profile_name TEXT PRIMARY KEY,
        callsign TEXT NOT NULL,
        locator TEXT NOT NULL,
        operator_name TEXT,
        qth_name TEXT,
        iota TEXT,
        sota TEXT,
        sig TEXT,
        sig_info TEXT,
        vucc TEXT
);
