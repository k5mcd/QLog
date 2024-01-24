ALTER TABLE rig_profiles ADD driver INTEGER DEFAULT 0;

UPDATE rig_profiles SET driver = 1;
