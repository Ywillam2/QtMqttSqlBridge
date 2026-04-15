#include "sqlmanagerwrapper.h"
#include "dbmanager.h"
SqlManagerWrapper::SqlManagerWrapper(QObject *parent)
    : QObject(parent), sqlManager(DbManager::instance())
{

}

bool SqlManagerWrapper::connect(const QString& host,
                                const QString& username,
                                const QString& password,
                                const QString& database,
                                int port)
{
    return sqlManager.connect(host, username, password, database, port);
}

void SqlManagerWrapper::disconnect()
{
    sqlManager.disconnect();
}

bool SqlManagerWrapper::isConnected() const
{
    return sqlManager.isConnected();
}

QString SqlManagerWrapper::lastError() const
{
    return sqlManager.lastError();
}