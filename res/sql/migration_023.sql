CREATE TABLE contacts_qsl_cards (
        contactid INTEGER REFERENCES contacts("id") ON DELETE CASCADE,
        "source" TEXT,
        name TEXT,
        "data" TEXT);

CREATE UNIQUE INDEX IF NOT EXISTS contacts_qsl_id_idx ON contacts_qsl_cards(contactid, "source", name);
