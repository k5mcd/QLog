CREATE TABLE IF NOT EXISTS log_param (
        name TEXT PRIMARY KEY,
        value TEXT
);

CREATE TABLE IF NOT EXISTS rig_profiles (
        profile_name TEXT PRIMARY KEY,
        model NUMBER NOT NULL,
        port_pathname TEXT,
        hostname TEXT,
        netport NUMBER,
        baudrate NUMBER,
        databits NUMBER,
        stopbits REAL,
        flowcontrol TEXT,
        parity TEXT,
        pollinterval INTEGER,
        txfreq_start REAL,
        txfreq_end REAL,
        get_freq INTEGER,
        get_mode INTEGER,
        get_vfo  INTEGER,
        get_pwr  INTEGER
);

CREATE TABLE IF NOT EXISTS rot_profiles (
        profile_name TEXT PRIMARY KEY,
        model NUMBER NOT NULL,
        port_pathname TEXT,
        hostname TEXT,
        netport NUMBER,
        baudrate NUMBER,
        databits NUMBER,
        stopbits REAL,
        flowcontrol TEXT,
        parity TEXT
);

CREATE TABLE IF NOT EXISTS ant_profiles (
        profile_name TEXT PRIMARY KEY,
        desc TEXT
);
