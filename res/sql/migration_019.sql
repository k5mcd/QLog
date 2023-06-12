UPDATE contacts SET mode = 'MFSK', submode = 'FT4' WHERE mode = 'FT4';
UPDATE contacts SET mode = 'MFSK', submode = 'FST4' WHERE mode = 'FST4';
UPDATE contacts SET mode = 'MFSK', submode = 'FST4W' WHERE mode = 'FST4W';
UPDATE contacts SET mode = 'MFSK', submode = 'Q65' WHERE mode = 'Q65';

UPDATE modes SET name = 'CONTESTI' WHERE name = 'CONTESTIA';
