#include "GenericCallbook.h"

GenericCallbook::GenericCallbook(QObject *parent) : QObject(parent)
{

}

const QString GenericCallbook::SECURE_STORAGE_KEY = "QLog:Callbook";
const QString GenericCallbook::CONFIG_USERNAME_KEY = "callbook/username";
const QString GenericCallbook::CONFIG_SELECTED_CALLBOOK_KEY = "callbook/selected";
