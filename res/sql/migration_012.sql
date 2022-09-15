CREATE TABLE IF NOT EXISTS cwkey_profiles(
        profile_name TEXT PRIMARY KEY,
        model NUMBER NOT NULL,
        default_speed NUMBER NOT NULL,
        key_mode NUMBER,
        port_pathname TEXT,
        baudrate NUMBER
);

CREATE TABLE IF NOT EXISTS cwshortcut_profiles(
        profile_name TEXT PRIMARY KEY,
        f1_short  TEXT,
        f1_macro  TEXT,
        f2_short  TEXT,
        f2_macro  TEXT,
        f3_short  TEXT,
        f3_macro  TEXT,
        f4_short  TEXT,
        f4_macro  TEXT,
        f5_short  TEXT,
        f5_macro  TEXT,
        f6_short  TEXT,
        f6_macro  TEXT,
        f7_short  TEXT,
        f7_macro  TEXT
);

INSERT INTO cwshortcut_profiles(
profile_name,
f1_short, f1_macro,
f2_short, f2_macro,
f3_short, f3_macro,
f4_short, f4_macro,
f5_short, f5_macro
)
VALUES (
"Run",
"CQ", "CQ CQ CQ DE <MYCALL> <MYCALL> <MYCALL> K",
"QRZ?", "QRZ?",
"AGN?", "AGN?",
"Text", "<DXCALL> DE <MYCALL> <GREETING> OM TNX FER CALL UR RST <RST> <RST> NAME <MYNAME> <MYNAME> QTH <MYQTH> <MYQTH> HW CPI? <DXCALL> DE <MYCALL> K",
"End", "<DXCALL> DE <MYCALL> FM 73 <DXCALL> DE <MYCALL> K"
);


ALTER TABLE rig_profiles ADD get_key_speed INTEGER DEFAULT 1;
ALTER TABLE rig_profiles ADD assigned_cw_key TEXT DEFAULT ' ';
