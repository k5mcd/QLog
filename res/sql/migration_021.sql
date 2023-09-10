CREATE TABLE IF NOT EXISTS chat_highlight_rules(
   rule_name TEXT PRIMARY KEY,
   room_id TEXT,
   enabled INTEGER,
   rule_definition TEXT
);
