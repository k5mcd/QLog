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
