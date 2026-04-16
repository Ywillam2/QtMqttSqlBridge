#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

#include "mqttdbtestbridge.h"
#include "mqttmanager.h"
#include "sqlmanagerwrapper.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onMqttConnectClicked();
    void onDbConnectClicked();
    void onMqttConnected();
    void onMqttDisconnected();
    void onMqttError(const QString &message);
    void onRefreshDatabases();
    void onUseDatabase();
    void onRefreshTables();
    void onDatabaseComboActivated(int index);
    void onCreateLogTable();
    void onAddSubscription();
    void onRemoveSubscription();
    void onCreateTableFromUi();
    void onExecuteRawSql();
    void onLoadSelectedTableSql();
    void onRunCrud();
    void onLogSubscriptionsChanged();

private:
    struct Ui;
    std::unique_ptr<Ui> m_ui;

    SqlManagerWrapper m_sql;
    MqttManager m_mqtt;
    MqttDbTestBridge m_bridge;

    void buildUi();
    void wireSignals();

    void appendLog(const QString &line);
    void setSqlResultText(const QString &text);
    void syncBridgeSettings();
    void refreshConnectionLabels();
    void updateSubscriptionList();
    void updateTableContextLabels();

    QString currentDatabaseName() const;
    QString currentTableName() const;
    bool ensureWorkingDatabase();
    bool currentTargetTableExists();
    QVariantMap parseKeyValueLines(const QString &text) const;
    QVariant parseScalar(const QString &text) const;
    QString formatRows(const QVariantList &rows) const;
    static int parsePort(const QString &text, int fallback);
    static int parseIntOrDefault(const QString &text, int fallback);
    void showInfoDialog(const QString &title, const QString &message);
};

#endif // MAINWINDOW_H
