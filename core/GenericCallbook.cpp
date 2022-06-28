#include "GenericCallbook.h"

GenericCallbook::GenericCallbook(QObject *parent) :
    QObject(parent)
{

}

const QString GenericCallbook::SECURE_STORAGE_KEY = "Callbook";
const QString GenericCallbook::CONFIG_USERNAME_KEY = "genericcallbook/username";
const QString GenericCallbook::CONFIG_PRIMARY_CALLBOOK_KEY = "callbook/primary";
const QString GenericCallbook::CONFIG_SECONDARY_CALLBOOK_KEY = "callbook/secondary";
const QString GenericCallbook::CALLBOOK_NAME = "none";
