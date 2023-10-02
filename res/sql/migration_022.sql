ALTER TABLE newcontact_layout_profiles RENAME TO main_layout_profiles;

ALTER TABLE main_layout_profiles ADD detail_col_A TEXT;
ALTER TABLE main_layout_profiles ADD detail_col_B TEXT;
ALTER TABLE main_layout_profiles ADD detail_col_C TEXT;
ALTER TABLE main_layout_profiles ADD main_geometry TEXT;
ALTER TABLE main_layout_profiles ADD main_state TEXT;

