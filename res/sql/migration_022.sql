ALTER TABLE newcontact_layout_profiles RENAME TO main_layout_profiles;

ALTER TABLE main_layout_profiles ADD detail_col_A TEXT;
ALTER TABLE main_layout_profiles ADD detail_col_B TEXT;
ALTER TABLE main_layout_profiles ADD detail_col_C TEXT;
ALTER TABLE main_layout_profiles ADD main_geometry TEXT;
ALTER TABLE main_layout_profiles ADD main_state TEXT;

DROP TABLE IF EXISTS contacts_old;

CREATE INDEX IF NOT EXISTS start_time_idx ON contacts(start_time);
CREATE INDEX IF NOT EXISTS band_idx ON contacts(band);
CREATE INDEX IF NOT EXISTS mode_idx ON contacts(mode);

CREATE TABLE IF NOT EXISTS adif_enum_primary_subdivision(
        code TEXT,
        subdivision_name  TEXT,
        dxcc INTEGER,
        PRIMARY KEY (code, dxcc)
);

INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('AK', 'Alaska', 6);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('HI', 'Hawaii', 110);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('CT', 'Connecticut', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('ME', 'Maine', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('MA', 'Massachusetts', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('NH', 'New Hampshire', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('RI', 'Rhode Island', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('VT', 'Vermont', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('NJ', 'New Jersey', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('NY', 'New York', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('DE', 'Delaware', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('DC', 'District of Columbia', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('MD', 'Maryland', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('PA', 'Pennsylvania', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('AL', 'Alabama', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('FL', 'Florida', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('GA', 'Georgia', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('KY', 'Kentucky', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('NC', 'North Carolina', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('SC', 'South Carolina', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('TN', 'Tennessee', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('VA', 'Virginia', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('AR', 'Arkansas', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('LA', 'Louisiana', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('MS', 'Mississippi', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('NM', 'New Mexico', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('OK', 'Oklahoma', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('TX', 'Texas', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('CA', 'California', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('AZ', 'Arizona', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('ID', 'Idaho', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('MT', 'Montana', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('NV', 'Nevada', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('OR', 'Oregon', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('UT', 'Utah', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('WA', 'Washington', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('WY', 'Wyoming', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('MI', 'Michigan', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('OH', 'Ohio', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('WV', 'West Virginia', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('IL', 'Illinois', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('IN', 'Indiana', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('WI', 'Wisconsin', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('CO', 'Colorado', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('IA', 'Iowa', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('KS', 'Kansas', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('MN', 'Minnesota', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('MO', 'Missouri', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('NE', 'Nebraska', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('ND', 'North Dakota', 291);
INSERT INTO adif_enum_primary_subdivision(code, subdivision_name, dxcc) values ('SD', 'South Dakota', 291);

