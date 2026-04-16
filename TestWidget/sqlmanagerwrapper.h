#ifndef SQLMANAGERWRAPPER_H
#define SQLMANAGERWRAPPER_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include "dbmanager.h"

/*
 * 与 Test 工程一致：为界面层提供数据库操作的 QVariant 友好接口。
 */
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

    Q_INVOKABLE bool beginTransaction();
    Q_INVOKABLE bool commitTransaction();
    Q_INVOKABLE bool rollbackTransaction();
    Q_INVOKABLE bool inTransaction() const;

    Q_INVOKABLE bool createDatabase(const QString& databaseName);
    Q_INVOKABLE bool dropDatabase(const QString& databaseName);
    Q_INVOKABLE bool useDatabase(const QString& databaseName);
    Q_INVOKABLE bool isDatabaseExists(const QString& databaseName) const;
    Q_INVOKABLE QStringList listDatabases();
    Q_INVOKABLE QStringList listTables(const QString &databaseName = "");

    Q_INVOKABLE bool createTable(const QString &tableName, const QVariantMap &fields);
    Q_INVOKABLE bool dropTable(const QString &tableName);
    Q_INVOKABLE bool isTableExists(const QString &tableName) const;
    Q_INVOKABLE bool insertRecord(const QString &tableName, const QVariantMap &values);
    Q_INVOKABLE bool updateRecord(const QString &tableName,
                                  const QVariantMap &values,
                                  const QString &condition);

    Q_INVOKABLE bool insert(const QString &tableName,
                            const QMap<QString, QVariant> &values);
    Q_INVOKABLE bool insertBatch(const QString &tableName,
                                 const QList<QMap<QString, QVariant>> &valuesList);
    Q_INVOKABLE bool update(const QString &tableName,
                            const QMap<QString, QVariant> &values,
                            const QString &condition);
    Q_INVOKABLE bool remove(const QString &tableName, const QString &condition);
    Q_INVOKABLE bool exec(const QString &sql);
    Q_INVOKABLE QVariantList query(const QString &sql);
    Q_INVOKABLE QVariantList select(
        const QString &tableName,
        const QString &fields = "*",
        const QString &condition = "",
        const QString &orderBy = "",
        int limit = -1,
        int offset = 0
        );
    Q_INVOKABLE QVariant lastInsertId() const;

private:
    DbManager& sqlManager;
};

#endif // SQLMANAGERWRAPPER_H
