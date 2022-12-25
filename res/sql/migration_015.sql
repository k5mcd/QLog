UPDATE modes SET submodes = '["C4FM", "DMR", "DSTAR", "FREEDV", "M17" ]' WHERE name = 'DIGITALVOICE';

INSERT INTO bands (name, start_freq, end_freq, enabled) VALUES
('submm',300000.0, 7500000.0, 0);
