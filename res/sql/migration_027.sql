ALTER TABLE bands ADD sat_designator TEXT;

UPDATE bands SET sat_designator='H' WHERE lower(name) = '15m';
UPDATE bands SET sat_designator='A' WHERE lower(name) = '10m';
UPDATE bands SET sat_designator='V' WHERE lower(name) = '2m';
UPDATE bands SET sat_designator='U' WHERE lower(name) = '70cm';
UPDATE bands SET sat_designator='L' WHERE lower(name) = '23cm';
UPDATE bands SET sat_designator='S' WHERE lower(name) = '13cm';
UPDATE bands SET sat_designator='S2' WHERE lower(name) = '9cm';
UPDATE bands SET sat_designator='C' WHERE lower(name) = '6cm';
UPDATE bands SET sat_designator='X' WHERE lower(name) = '3cm';
UPDATE bands SET sat_designator='K' WHERE lower(name) = '1.25cm';
UPDATE bands SET sat_designator='R' WHERE lower(name) = '6mm';
