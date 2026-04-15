#ifndef SQLMANAGERWRAPPER_H
#define SQLMANAGERWRAPPER_H

#include <QObject>
#include "dbmanager.h"
class SqlManagerWrapper : public QObject
{
    Q_OBJECT
public:
    explicit SqlManagerWrapper(QObject *parent = nullptr);
    Q_INVOKABLE bool connect(const QString& host,
                             const QString& username,
                             const QString& password,
                             const QString& database="",
                             int port=3306);
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE bool isConnected() const;
    Q_INVOKABLE QString lastError() const;
signals:

private:
    DbManager& sqlManager;
};

#endif // SQLMANAGERWRAPPER_H
