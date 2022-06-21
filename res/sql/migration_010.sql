ALTER TABLE bands ADD last_seen_freq FLOAT;

DELETE FROM modes WHERE name IN ('C4FM', 'DMR', 'DSTAR');
UPDATE modes SET submodes = '["C4FM", "DMR", "DSTAR"]' WHERE name = 'DIGITALVOICE';

INSERT INTO modes (name, rprt, dxcc, enabled, submodes) VALUES
('DYNAMIC', NULL, 'DIGITAL', false, '["VARA HF", "VARA SATELLITE", "VARA FM 1200", "VARA FM 9600"]')
ON CONFLICT DO NOTHING;
