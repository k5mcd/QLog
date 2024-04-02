ALTER TABLE rot_profiles ADD driver INTEGER DEFAULT 0;

UPDATE rot_profiles SET driver = 1;
