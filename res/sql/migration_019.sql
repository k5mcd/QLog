UPDATE contacts SET clublog_qso_upload_status = 'M' WHERE mode IN ('FT4', 'FST4', 'FST4W', 'Q65') AND UPPER(clublog_qso_upload_status) = 'Y';
UPDATE contacts SET qrzcom_qso_upload_status = 'M' WHERE mode IN ('FT4', 'FST4', 'FST4W', 'Q65') AND UPPER(qrzcom_qso_upload_status) = 'Y';

UPDATE contacts SET mode = 'MFSK', submode = 'FT4' WHERE mode = 'FT4';
UPDATE contacts SET mode = 'MFSK', submode = 'FST4' WHERE mode = 'FST4';
UPDATE contacts SET mode = 'MFSK', submode = 'FST4W' WHERE mode = 'FST4W';
UPDATE contacts SET mode = 'MFSK', submode = 'Q65' WHERE mode = 'Q65';
