CREATE TABLE IF NOT EXISTS schema_versions (
        version INTEGER PRIMARY KEY,
        updated TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS contacts (
        id INTEGER PRIMARY KEY,
        start_time TEXT,
        end_time TEXT,
        callsign TEXT NOT NULL,
        rst_sent TEXT,
        rst_rcvd TEXT,
        freq REAL,
        band TEXT,
        mode TEXT,
        submode TEXT,
        name TEXT,
        qth TEXT,
        gridsquare TEXY,
        dxcc INTEGER,
        country TEXT,
        cont TEXT,
        cqz INTEGER,
        ituz INTEGER,
        pfx TEXT,
        state TEXT,
        cnty TEXT,
        iota TEXT,
        qsl_rcvd CHECK(qsl_rcvd IN ('N', 'Y', 'R', 'I')) NOT NULL DEFAULT 'N',
        qsl_rdate TEXT,
        qsl_sent CHECK(qsl_sent IN ('N', 'Y', 'R', 'Q', 'I')) NOT NULL DEFAULT 'N',
        qsl_sdate TEXT,
        lotw_qsl_rcvd CHECK(qsl_rcvd IN ('N', 'Y', 'R', 'I')) NOT NULL DEFAULT 'N',
        lotw_qslrdate TEXT,
        lotw_qsl_sent CHECK(qsl_sent IN ('N', 'Y', 'R', 'Q', 'I')) NOT NULL DEFAULT 'N',
        lotw_qslsdate TEXT,
        tx_pwr REAL,
        fields JSON
);


CREATE TABLE IF NOT EXISTS bands (
        id INTEGER PRIMARY KEY,
        name TEXT UNIQUE NOT NULL,
        start_freq FLOAT,
        end_freq FLOAT,
        enabled BOOLEAN
);

CREATE TABLE IF NOT EXISTS modes (
        id INTEGER PRIMARY KEY,
        name TEXT UNIQUE NOT NULL,
        submodes JSON,
        rprt TEXT,
        dxcc TEXT CHECK(dxcc IN ('CW', 'PHONE', 'DIGITAL')) NOT NULL,
        enabled INTEGER
);

CREATE TABLE IF NOT EXISTS dxcc_entities (
        id INTEGER PRIMARY KEY,
        name TEXT NOT NULL,
        prefix TEXT,
        cont TEXT,
        cqz INTEGER,
        ituz INTEGER,
        lat REAL,
        lon REAL,
        tz REAL
);

CREATE TABLE IF NOT EXISTS dxcc_prefixes (
        id INTEGER PRIMARY KEY,
        prefix TEXT UNIQUE NOT NULL,
        exact INTEGER,
        dxcc INTEGER REFERENCES dxcc_entities(id),
        cqz INTEGER,
        ituz INTEGER,
        cont TEXT,
        lat REAL,
        lon REAL
);

CREATE INDEX prefix_idx ON dxcc_prefixes(prefix);
CREATE INDEX callsign_idx ON contacts(callsign);


INSERT INTO modes (name, rprt, dxcc, enabled, submodes) VALUES
('CW', '599', 'CW', true, NULL),
('SSB', '59', 'PHONE', true, '["LSB", "USB"]'),
('AM', '59', 'PHONE', true, NULL),
('FM', '59', 'PHONE', true, NULL),
('PSK', '599', 'DIGITAL', true, '[
        "PSK31",
        "PSK63",
        "PSK63F",
        "PSK125",
        "PSK250",
        "PSK500",
        "PSK1000",
        "QPSK31",
        "QPSK63",
        "QPSK125",
        "QPSK250",
        "QPSK500"
]'),
('RTTY', '599', 'DIGITAL', true, NULL),
('MFSK', '599', 'DIGITAL', true, '[
        "FT4",
        "MFSK4",
        "MFSK8",
        "MFSK11",
        "MFSK16",
        "MFSK22",
        "MFSK31",
        "MFSK32"
]'),
('OLIVIA', NULL, 'DIGITAL', true, '[
        "OLIVIA 4/125",
        "OLIVIA 4/250",
        "OLIVIA 8/250",
        "OLIVIA 8/500",
        "OLIVIA 16/500",
        "OLIVIA 16/1000",
        "OLIVIA 32/1000"
]'),
('JT65', '-1', 'DIGITAL', true, '[
        "JT65A",
        "JT65B",
        "JT65B2",
        "JT65C",
        "JT65C2"
]'),
('JT9', '-1', 'DIGITAL', true, '[
        "JT9-1",
        "JT9-2",
        "JT9-5",
        "JT9-10",
        "JT9-30"
]'),
('FT8', '-1', 'DIGITAL', true, NULL),
('HELL', NULL, 'DIGITAL', true, '[
        "FMHELL",
        "FSKHELL",
        "HELL80",
        "HFSK",
        "PSKHELL"
]'),
('CONTESTIA', NULL, 'DIGITAL', true, NULL),
('DOMINO', NULL, 'DIGITAL', true, '[
        "DOMINOEX",
        "DOMINOF"
]'),
('MT63', NULL, 'DIGITAL', true, NULL),
('JT6M', '26', 'DIGITAL', true, NULL),
('JTMSK', '-1', 'DIGITAL', true, NULL),
('MSK144', '0', 'DIGITAL', true, NULL),
('FSK441', '26', 'DIGITAL', true, NULL),
('DIGITALVOICE', '59', 'PHONE', true, NULL),
('DSTAR', '59', 'PHONE', true, NULL),
('PKT', NULL, 'DIGITAL', true, NULL),
('ATV', NULL, 'DIGITAL', true, NULL),
('SSTV', NULL, 'DIGITAL', true, NULL);

INSERT INTO bands (name, start_freq, end_freq, enabled) VALUES
('2190m', 0.136, 0.137, false),
('630m', 0.472, 0.479, false),
('560m', 0.501, 0.504, false),
('160m', 1.8, 2.0, true),
('80m', 3.5, 4.0, true),
('60m', 5.102, 5.4065, true),
('40m', 7.0, 7.3, true),
('30m', 10.1, 10.15, true),
('20m', 14.0, 14.35, true),
('17m', 18.068, 18.168, true),
('15m', 21.0, 21.45, true),
('12m', 24.89, 24.99, true),
('10m', 28.0, 29.7, true),
('6m', 50.0, 54.0, true),
('4m', 70.0, 71.0, true),
('2m', 144.0, 148.0, true),
('1.25m', 222.0, 225.0, false),
('70cm', 420.0, 450.0, true),
('33cm', 902.0, 928.0, false),
('23cm', 1240.0, 1300.0, false),
('13cm', 2300.0, 2450.0, false),
('9cm', 3300.0, 3500.0, false),
('6cm', 5650.0, 5925.0, false),
('3cm', 10000.0, 10500.0, false),
('1.25cm', 24000.0, 24250.0, false),
('6mm', 47000.0, 47200.0, false),
('4mm', 75500.0, 81000.0, false),
('2.5mm', 119980.0, 120020.0, false),
('2mm', 142000.0, 149000.0, false),
('1mm', 241000.0, 250000.0, false);
