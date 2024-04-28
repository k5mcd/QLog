ALTER TABLE rot_profiles ADD driver INTEGER DEFAULT 0;
ALTER TABLE station_profiles ADD ituz INTEGER;
ALTER TABLE station_profiles ADD cqz INTEGER;
ALTER TABLE station_profiles ADD dxcc INTEGER;
ALTER TABLE station_profiles ADD country TEXT;

UPDATE rot_profiles SET driver = 1;
