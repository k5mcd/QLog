CREATE TABLE IF NOT EXISTS sota_summits(
        summit_code TEXT PRIMARY KEY,
        association_name TEXT,
        region_name TEXT,
        summit_name TEXT,
        altm INTEGER,
        altft INTEGER,
        gridref1 REAL,
        gridref2 REAL,
        longitude REAL,
        latitude REAL,
        points INTEGER,
        bonus_points INTEGER,
        valid_from TEXT,
        valid_to TEXT
);

CREATE TABLE IF NOT EXISTS wwff_directory(
        reference TEXT PRIMARY KEY,
        status TEXT,
        name TEXT,
        program TEXT,
        dxcc TEXT,
        state TEXT,
        county TEXT,
        continent TEXT,
        iota TEXT,
        iaruLocator TEXT,
        latitude REAL,
        longitude REAL,
        iucncat TEXT,
        valid_from TEXT,
        valid_to TEXT
);
