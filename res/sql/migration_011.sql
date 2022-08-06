ALTER TABLE rig_profiles ADD qsy_wiping INTEGER DEFAULT 0;

UPDATE rig_profiles SET qsy_wiping = get_freq;
