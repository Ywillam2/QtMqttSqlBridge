#include "mainwindow.h"

#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextCursor>
#include <QFrame>

#include <QFont>

struct MainWindow::Ui {
    QLabel *mqttStatus = nullptr;
    QLineEdit *mqttHost = nullptr;
    QLineEdit *mqttPort = nullptr;
    QLineEdit *mqttUser = nullptr;
    QLineEdit *mqttPassword = nullptr;
    QLineEdit *mqttClientId = nullptr;
    QPushButton *btnMqttConnect = nullptr;

    QLabel *dbStatus = nullptr;
    QLineEdit *dbHost = nullptr;
    QLineEdit *dbPort = nullptr;
    QLineEdit *dbName = nullptr;
    QLineEdit *dbUser = nullptr;
    QLineEdit *dbPassword = nullptr;
    QPushButton *btnDbConnect = nullptr;

    QComboBox *databaseCombo = nullptr;
    QComboBox *tableCombo = nullptr;
    QPushButton *btnRefreshDbs = nullptr;
    QPushButton *btnUseDb = nullptr;
    QPushButton *btnRefreshTables = nullptr;

    QLabel *lblSubscriptionTargetTable = nullptr;
    QPushButton *btnCreateLogTable = nullptr;
    QLineEdit *topicFilter = nullptr;
    QPushButton *btnSubscribe = nullptr;
    QListWidget *subscriptionList = nullptr;
    QPushButton *btnRemoveSubscription = nullptr;

    QLineEdit *newTableName = nullptr;
    QPlainTextEdit *newTableColumns = nullptr;

    QPlainTextEdit *sqlEditor = nullptr;

    QComboBox *crudOp = nullptr;
    QLineEdit *crudFields = nullptr;
    QLineEdit *crudLimit = nullptr;
    QLineEdit *crudCondition = nullptr;
    QLineEdit *crudOrderBy = nullptr;
    QPlainTextEdit *crudValues = nullptr;
    QLineEdit *crudTableDisplay = nullptr;

    QPlainTextEdit *sqlResult = nullptr;
    QPlainTextEdit *logArea = nullptr;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(std::make_unique<Ui>())
    , m_sql()
    , m_mqtt()
    , m_bridge(&m_mqtt, &m_sql)
{
    setWindowTitle(tr("MQTT Message To Database Tester"));
    resize(1100, 820);
    setMinimumSize(980, 760);

    buildUi();
    wireSignals();

    appendLog(QStringLiteral("[widget] Application started"));
    refreshConnectionLabels();
    updateSubscriptionList();
    syncBridgeSettings();
    updateTableContextLabels();
}

MainWindow::~MainWindow() = default;

void MainWindow::appendLog(const QString &line)
{
    QPlainTextEdit *log = m_ui->logArea;
    if (!log)
        return;
    if (!log->toPlainText().isEmpty())
        log->appendPlainText(QString());
    log->appendPlainText(line);
    log->moveCursor(QTextCursor::End);
}

void MainWindow::setSqlResultText(const QString &text)
{
    if (m_ui->sqlResult)
        m_ui->sqlResult->setPlainText(text);
    appendLog(QStringLiteral("[sql-result] updated"));
}

void MainWindow::showInfoDialog(const QString &title, const QString &message)
{
    QMessageBox::information(this, title, message);
}

QString MainWindow::currentDatabaseName() const
{
    const QString comboText = m_ui->databaseCombo->currentText().trimmed();
    if (!comboText.isEmpty())
        return comboText;
    return m_ui->dbName->text().trimmed();
}

QString MainWindow::currentTableName() const
{
    return m_ui->tableCombo->currentText().trimmed();
}

void MainWindow::syncBridgeSettings()
{
    const QString targetTable = currentTableName();
    if (m_bridge.messageLogTable() != targetTable)
        m_bridge.setMessageLogTable(targetTable);
}

void MainWindow::refreshConnectionLabels()
{
    const bool mqttOk = m_mqtt.isConnected();
    const bool dbOk = m_sql.isConnected();
    m_ui->mqttStatus->setText(mqttOk ? tr("Status: Connected") : tr("Status: Disconnected"));
    m_ui->dbStatus->setText(dbOk ? tr("Status: Connected") : tr("Status: Disconnected"));
    m_ui->btnMqttConnect->setText(mqttOk ? tr("Disconnect") : tr("Connect"));
    m_ui->btnDbConnect->setText(dbOk ? tr("Disconnect") : tr("Connect"));

    const bool hasTable = !currentTableName().isEmpty();
    m_ui->btnSubscribe->setEnabled(mqttOk && dbOk && hasTable);
    m_ui->btnCreateLogTable->setEnabled(dbOk && hasTable);
}

void MainWindow::updateSubscriptionList()
{
    m_ui->subscriptionList->clear();
    for (const QString &t : m_bridge.logSubscriptions())
        m_ui->subscriptionList->addItem(t);
}

void MainWindow::updateTableContextLabels()
{
    const QString t = currentTableName();
    const QString display = t.isEmpty() ? tr("Choose or enter a table above") : t;
    m_ui->lblSubscriptionTargetTable->setText(display);
    m_ui->crudTableDisplay->setText(t);
}

bool MainWindow::ensureWorkingDatabase()
{
    const QString databaseName = currentDatabaseName();
    if (databaseName.isEmpty()) {
        appendLog(tr("Please choose a database first"));
        return false;
    }
    m_ui->dbName->setText(databaseName);
    if (!m_sql.useDatabase(databaseName)) {
        appendLog(tr("Use database failed:") + QLatin1Char(' ') + m_sql.lastError());
        return false;
    }
    return true;
}

bool MainWindow::currentTargetTableExists()
{
    const QString tableName = currentTableName();
    const QString databaseName = currentDatabaseName();
    if (!m_sql.isConnected() || tableName.isEmpty() || databaseName.isEmpty())
        return false;
    if (!m_sql.useDatabase(databaseName))
        return false;
    return m_sql.isTableExists(tableName);
}

int MainWindow::parsePort(const QString &text, int fallback)
{
    bool ok = false;
    const int v = text.trimmed().toInt(&ok);
    return ok ? v : fallback;
}

int MainWindow::parseIntOrDefault(const QString &text, int fallback)
{
    bool ok = false;
    const int v = text.trimmed().toInt(&ok);
    return ok ? v : fallback;
}

QVariant MainWindow::parseScalar(const QString &text) const
{
    const QString value = text.trimmed();
    if (value.isEmpty())
        return QString();
    if (value.compare(QStringLiteral("null"), Qt::CaseInsensitive) == 0)
        return QVariant();
    if (value == QStringLiteral("true"))
        return true;
    if (value == QStringLiteral("false"))
        return false;
    bool okNum = false;
    const double numberValue = value.toDouble(&okNum);
    if (okNum)
        return numberValue;
    return value;
}

QVariantMap MainWindow::parseKeyValueLines(const QString &text) const
{
    QVariantMap result;
    const QStringList lines = text.split(QLatin1Char('\n'));
    for (QString line : lines) {
        line = line.trimmed();
        if (line.isEmpty())
            continue;
        int separator = line.indexOf(QLatin1Char('='));
        if (separator < 0)
            separator = line.indexOf(QLatin1Char(':'));
        if (separator < 0)
            continue;
        const QString key = line.left(separator).trimmed();
        const QString value = line.mid(separator + 1).trimmed();
        if (!key.isEmpty())
            result.insert(key, parseScalar(value));
    }
    return result;
}

QString MainWindow::formatRows(const QVariantList &rows) const
{
    if (rows.isEmpty())
        return QStringLiteral("No rows returned.");

    QStringList lines;
    lines.reserve(rows.size());
    for (const QVariant &rowVar : rows) {
        const QVariantMap row = rowVar.toMap();
        if (row.isEmpty())
            continue;
        QStringList parts;
        parts.reserve(row.size());
        for (auto it = row.constBegin(); it != row.constEnd(); ++it)
            parts.append(it.key() + QLatin1Char('=') + it.value().toString());
        lines.append(parts.join(QStringLiteral(" | ")));
    }
    return lines.join(QLatin1Char('\n'));
}

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *root = new QHBoxLayout(central);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(12);

    auto *left = new QWidget;
    left->setMinimumWidth(250);
    left->setMaximumWidth(300);
    auto *leftLay = new QVBoxLayout(left);
    leftLay->setSpacing(8);

    auto addFormRow = [](QGridLayout *grid, int row, const QString &label, QWidget *field) {
        grid->addWidget(new QLabel(label), row, 0);
        grid->addWidget(field, row, 1);
    };

    {
        auto *gb = new QGroupBox(tr("MqttConnection"));
        auto *v = new QVBoxLayout(gb);
        v->setSpacing(6);
        m_ui->mqttStatus = new QLabel(tr("Status: Disconnected"));
        m_ui->mqttStatus->setStyleSheet(QStringLiteral("font-weight:bold;"));
        v->addWidget(m_ui->mqttStatus);
        auto *grid = new QGridLayout;
        grid->setHorizontalSpacing(8);
        grid->setVerticalSpacing(6);
        m_ui->mqttHost = new QLineEdit(QStringLiteral("127.0.0.1"));
        m_ui->mqttPort = new QLineEdit(QStringLiteral("1883"));
        m_ui->mqttUser = new QLineEdit;
        m_ui->mqttPassword = new QLineEdit;
        m_ui->mqttPassword->setEchoMode(QLineEdit::Password);
        m_ui->mqttClientId = new QLineEdit(QStringLiteral("appTestClient"));
        int r = 0;
        addFormRow(grid, r++, tr("Host"), m_ui->mqttHost);
        addFormRow(grid, r++, tr("Port"), m_ui->mqttPort);
        addFormRow(grid, r++, tr("Username"), m_ui->mqttUser);
        addFormRow(grid, r++, tr("Password"), m_ui->mqttPassword);
        addFormRow(grid, r++, tr("ClientId"), m_ui->mqttClientId);
        v->addLayout(grid);
        m_ui->btnMqttConnect = new QPushButton(tr("Connect"));
        auto *h = new QHBoxLayout;
        h->addStretch();
        h->addWidget(m_ui->btnMqttConnect);
        v->addLayout(h);
        leftLay->addWidget(gb);
    }

    {
        auto *gb = new QGroupBox(tr("DbConnection"));
        auto *v = new QVBoxLayout(gb);
        v->setSpacing(6);
        m_ui->dbStatus = new QLabel(tr("Status: Disconnected"));
        m_ui->dbStatus->setStyleSheet(QStringLiteral("font-weight:bold;"));
        v->addWidget(m_ui->dbStatus);
        auto *grid = new QGridLayout;
        grid->setHorizontalSpacing(8);
        grid->setVerticalSpacing(6);
        m_ui->dbHost = new QLineEdit(QStringLiteral("127.0.0.1"));
        m_ui->dbPort = new QLineEdit(QStringLiteral("3306"));
        m_ui->dbName = new QLineEdit;
        m_ui->dbUser = new QLineEdit(QStringLiteral("root"));
        m_ui->dbPassword = new QLineEdit;
        m_ui->dbPassword->setEchoMode(QLineEdit::Password);
        int r = 0;
        addFormRow(grid, r++, tr("Host"), m_ui->dbHost);
        addFormRow(grid, r++, tr("Port"), m_ui->dbPort);
        addFormRow(grid, r++, tr("Database"), m_ui->dbName);
        addFormRow(grid, r++, tr("Username"), m_ui->dbUser);
        addFormRow(grid, r++, tr("Password"), m_ui->dbPassword);
        v->addLayout(grid);
        m_ui->btnDbConnect = new QPushButton(tr("Connect"));
        auto *h = new QHBoxLayout;
        h->addStretch();
        h->addWidget(m_ui->btnDbConnect);
        v->addLayout(h);
        leftLay->addWidget(gb);
    }

    {
        auto *gb = new QGroupBox(tr("Database Browser"));
        auto *v = new QVBoxLayout(gb);
        v->setSpacing(6);
        v->addWidget(new QLabel(tr("Choose working database and table.")));
        v->addWidget(new QLabel(tr("Database")));
        m_ui->databaseCombo = new QComboBox;
        m_ui->databaseCombo->setEditable(true);
        v->addWidget(m_ui->databaseCombo);
        auto *row = new QHBoxLayout;
        m_ui->btnRefreshDbs = new QPushButton(tr("Refresh DBs"));
        m_ui->btnUseDb = new QPushButton(tr("Use DB"));
        row->addWidget(m_ui->btnRefreshDbs);
        row->addWidget(m_ui->btnUseDb);
        v->addLayout(row);
        v->addWidget(new QLabel(tr("Table")));
        m_ui->tableCombo = new QComboBox;
        m_ui->tableCombo->setEditable(true);
        v->addWidget(m_ui->tableCombo);
        m_ui->btnRefreshTables = new QPushButton(tr("Refresh Tables"));
        v->addWidget(m_ui->btnRefreshTables);
        leftLay->addWidget(gb);
    }

    leftLay->addStretch();

    auto *right = new QWidget;
    auto *rightLay = new QVBoxLayout(right);
    rightLay->setSpacing(12);

    auto *workspaceGb = new QGroupBox(tr("Data Workspace"));
    auto *workspaceOuter = new QVBoxLayout(workspaceGb);
    auto *tabs = new QTabWidget;

    {
        auto *tab = new QWidget;
        auto *lay = new QVBoxLayout(tab);
        lay->setSpacing(10);
        lay->addWidget(new QLabel(tr("Subscribe topic filters and write incoming MQTT messages into the current target table.")));
        auto *rowTarget = new QHBoxLayout;
        rowTarget->addWidget(new QLabel(tr("Target table")));
        m_ui->lblSubscriptionTargetTable = new QLabel;
        m_ui->lblSubscriptionTargetTable->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        rowTarget->addWidget(m_ui->lblSubscriptionTargetTable, 1);
        m_ui->btnCreateLogTable = new QPushButton(tr("Create target table"));
        rowTarget->addWidget(m_ui->btnCreateLogTable);
        lay->addLayout(rowTarget);

        auto *rowTopic = new QHBoxLayout;
        rowTopic->addWidget(new QLabel(tr("Topic filter")));
        m_ui->topicFilter = new QLineEdit;
        m_ui->topicFilter->setPlaceholderText(QStringLiteral("example/topic or sensors/#"));
        rowTopic->addWidget(m_ui->topicFilter, 1);
        m_ui->btnSubscribe = new QPushButton(tr("Subscribe"));
        rowTopic->addWidget(m_ui->btnSubscribe);
        lay->addLayout(rowTopic);

        lay->addWidget(new QLabel(tr("Current subscriptions")));
        m_ui->subscriptionList = new QListWidget;
        m_ui->subscriptionList->setMinimumHeight(160);
        lay->addWidget(m_ui->subscriptionList, 1);
        m_ui->btnRemoveSubscription = new QPushButton(tr("Remove selected subscription"));
        lay->addWidget(m_ui->btnRemoveSubscription);
        tabs->addTab(tab, tr("Subscription"));
    }

    {
        auto *tab = new QWidget;
        auto *lay = new QVBoxLayout(tab);
        lay->addWidget(new QLabel(tr("Enter the table name and column definitions, then create the table.")));
        auto *nameRow = new QHBoxLayout;
        nameRow->addWidget(new QLabel(tr("Table name")));
        m_ui->newTableName = new QLineEdit;
        m_ui->newTableName->setPlaceholderText(QStringLiteral("mqtt_messages"));
        nameRow->addWidget(m_ui->newTableName, 1);
        lay->addLayout(nameRow);
        lay->addWidget(new QLabel(tr("Column definitions (one line per column)")));
        m_ui->newTableColumns = new QPlainTextEdit;
        m_ui->newTableColumns->setPlaceholderText(
            QStringLiteral("id INT AUTO_INCREMENT PRIMARY KEY,\n"
                           "topic VARCHAR(512) NOT NULL,\n"
                           "payload JSON NOT NULL"));
        m_ui->newTableColumns->setMinimumHeight(140);
        lay->addWidget(m_ui->newTableColumns, 1);
        auto *execBtn = new QPushButton(tr("Generate && Execute"));
        connect(execBtn, &QPushButton::clicked, this, &MainWindow::onCreateTableFromUi);
        lay->addWidget(execBtn, 0, Qt::AlignLeft);
        tabs->addTab(tab, tr("Create Table"));
    }

    {
        auto *tab = new QWidget;
        auto *lay = new QVBoxLayout(tab);
        lay->addWidget(new QLabel(tr("Execute any SQL statement. Query results will be shown below.")));
        m_ui->sqlEditor = new QPlainTextEdit;
        m_ui->sqlEditor->setPlaceholderText(
            QStringLiteral("SELECT * FROM my_table LIMIT 10;\n\nUPDATE my_table SET topic='demo' WHERE id=1;"));
        m_ui->sqlEditor->setMinimumHeight(140);
        lay->addWidget(m_ui->sqlEditor, 1);
        auto *row = new QHBoxLayout;
        auto *b1 = new QPushButton(tr("Execute SQL"));
        connect(b1, &QPushButton::clicked, this, &MainWindow::onExecuteRawSql);
        auto *b2 = new QPushButton(tr("Load selected table"));
        connect(b2, &QPushButton::clicked, this, &MainWindow::onLoadSelectedTableSql);
        row->addWidget(b1);
        row->addWidget(b2);
        row->addStretch();
        lay->addLayout(row);
        tabs->addTab(tab, tr("SQL Console"));
    }

    {
        auto *scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        auto *inner = new QWidget;
        auto *lay = new QVBoxLayout(inner);
        lay->setSpacing(10);

        auto *opRow = new QHBoxLayout;
        opRow->addWidget(new QLabel(tr("Operation")));
        m_ui->crudOp = new QComboBox;
        m_ui->crudOp->addItems({QStringLiteral("Select"), QStringLiteral("Insert"), QStringLiteral("Update"), QStringLiteral("Delete")});
        opRow->addWidget(m_ui->crudOp);
        opRow->addWidget(new QLabel(tr("Target table")));
        m_ui->crudTableDisplay = new QLineEdit;
        m_ui->crudTableDisplay->setReadOnly(true);
        opRow->addWidget(m_ui->crudTableDisplay, 1);
        lay->addLayout(opRow);

        auto *fRow = new QHBoxLayout;
        fRow->addWidget(new QLabel(tr("Fields")));
        m_ui->crudFields = new QLineEdit;
        m_ui->crudFields->setPlaceholderText(QStringLiteral("* or id,name,topic"));
        fRow->addWidget(m_ui->crudFields, 1);
        fRow->addWidget(new QLabel(tr("Limit")));
        m_ui->crudLimit = new QLineEdit(QStringLiteral("-1"));
        m_ui->crudLimit->setMaximumWidth(80);
        fRow->addWidget(m_ui->crudLimit);
        lay->addLayout(fRow);

        auto *cRow = new QHBoxLayout;
        cRow->addWidget(new QLabel(tr("Condition")));
        m_ui->crudCondition = new QLineEdit;
        m_ui->crudCondition->setPlaceholderText(QStringLiteral("id = 1"));
        cRow->addWidget(m_ui->crudCondition, 1);
        lay->addLayout(cRow);

        auto *oRow = new QHBoxLayout;
        oRow->addWidget(new QLabel(tr("Order by")));
        m_ui->crudOrderBy = new QLineEdit;
        m_ui->crudOrderBy->setPlaceholderText(QStringLiteral("id DESC"));
        oRow->addWidget(m_ui->crudOrderBy, 1);
        lay->addLayout(oRow);

        lay->addWidget(new QLabel(tr("Values (key=value, one per line)")));
        m_ui->crudValues = new QPlainTextEdit;
        m_ui->crudValues->setPlaceholderText(QStringLiteral("topic=test/value\npayload=hello world\npriority=1"));
        m_ui->crudValues->setMinimumHeight(120);
        lay->addWidget(m_ui->crudValues);

        auto *run = new QPushButton(tr("Run CRUD"));
        connect(run, &QPushButton::clicked, this, &MainWindow::onRunCrud);
        lay->addWidget(run, 0, Qt::AlignLeft);

        scroll->setWidget(inner);
        tabs->addTab(scroll, tr("CRUD Helper"));
    }

    workspaceOuter->addWidget(tabs);

    auto *sqlGb = new QGroupBox(tr("SQL Result"));
    auto *sqlLay = new QVBoxLayout(sqlGb);
    m_ui->sqlResult = new QPlainTextEdit;
    m_ui->sqlResult->setReadOnly(true);
    m_ui->sqlResult->setMinimumHeight(100);
    sqlLay->addWidget(m_ui->sqlResult);

    auto *logGb = new QGroupBox(tr("Logs"));
    auto *logLay = new QVBoxLayout(logGb);
    m_ui->logArea = new QPlainTextEdit;
    m_ui->logArea->setReadOnly(true);
    m_ui->logArea->setMinimumHeight(140);
    m_ui->logArea->setFont(QFont(QStringLiteral("Consolas"), 9));
    logLay->addWidget(m_ui->logArea);

    rightLay->addWidget(workspaceGb, 2);
    rightLay->addWidget(sqlGb, 0);
    rightLay->addWidget(logGb, 1);

    root->addWidget(left, 0);
    root->addWidget(right, 1);
}

void MainWindow::wireSignals()
{
    connect(m_ui->btnMqttConnect, &QPushButton::clicked, this, &MainWindow::onMqttConnectClicked);
    connect(m_ui->btnDbConnect, &QPushButton::clicked, this, &MainWindow::onDbConnectClicked);
    connect(&m_mqtt, &MqttManager::connected, this, &MainWindow::onMqttConnected);
    connect(&m_mqtt, &MqttManager::disconnected, this, &MainWindow::onMqttDisconnected);
    connect(&m_mqtt, &MqttManager::error, this, &MainWindow::onMqttError);

    connect(m_ui->btnRefreshDbs, &QPushButton::clicked, this, &MainWindow::onRefreshDatabases);
    connect(m_ui->btnUseDb, &QPushButton::clicked, this, &MainWindow::onUseDatabase);
    connect(m_ui->btnRefreshTables, &QPushButton::clicked, this, &MainWindow::onRefreshTables);
    connect(m_ui->databaseCombo, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::onDatabaseComboActivated);
    connect(m_ui->tableCombo, &QComboBox::currentTextChanged, this, [this](const QString &) {
        syncBridgeSettings();
        updateTableContextLabels();
        refreshConnectionLabels();
    });

    connect(m_ui->btnCreateLogTable, &QPushButton::clicked, this, &MainWindow::onCreateLogTable);
    connect(m_ui->btnSubscribe, &QPushButton::clicked, this, &MainWindow::onAddSubscription);
    connect(m_ui->btnRemoveSubscription, &QPushButton::clicked, this, &MainWindow::onRemoveSubscription);
    connect(m_ui->topicFilter, &QLineEdit::returnPressed, this, &MainWindow::onAddSubscription);

    connect(&m_bridge, &MqttDbTestBridge::logSubscriptionsChanged, this, &MainWindow::onLogSubscriptionsChanged);
}

void MainWindow::onMqttConnectClicked()
{
    appendLog(QStringLiteral("[widget] connectMqtt() clicked"));
    if (m_mqtt.isConnected()) {
        m_mqtt.disconnectFromBroker();
        return;
    }
    m_mqtt.connectToBroker(
        m_ui->mqttHost->text().trimmed(),
        parsePort(m_ui->mqttPort->text(), 1883),
        m_ui->mqttClientId->text().trimmed(),
        m_ui->mqttUser->text().trimmed(),
        m_ui->mqttPassword->text());
}

void MainWindow::onDbConnectClicked()
{
    appendLog(QStringLiteral("[widget] connectDatabase() clicked"));
    if (m_sql.isConnected()) {
        m_sql.disconnect();
        appendLog(tr("Database disconnected"));
        refreshConnectionLabels();
        return;
    }
    const bool ok = m_sql.connect(
        m_ui->dbHost->text().trimmed(),
        m_ui->dbUser->text().trimmed(),
        m_ui->dbPassword->text(),
        m_ui->dbName->text().trimmed(),
        parsePort(m_ui->dbPort->text(), 3306));
    if (ok) {
        appendLog(tr("Database connected"));
        onRefreshDatabases();
        if (!m_ui->dbName->text().trimmed().isEmpty())
            onRefreshTables();
    } else {
        appendLog(tr("Database connect failed:") + QLatin1Char(' ') + m_sql.lastError());
    }
    refreshConnectionLabels();
}

void MainWindow::onMqttConnected()
{
    appendLog(QStringLiteral("[widget] signal mqttManager.connected"));
    appendLog(tr("MQTT connected"));
    refreshConnectionLabels();
}

void MainWindow::onMqttDisconnected()
{
    appendLog(QStringLiteral("[widget] signal mqttManager.disconnected"));
    appendLog(tr("MQTT disconnected"));
    refreshConnectionLabels();
}

void MainWindow::onMqttError(const QString &message)
{
    appendLog(QStringLiteral("[widget] signal mqttManager.error ") + message);
    appendLog(tr("MQTT error:") + QLatin1Char(' ') + message);
    refreshConnectionLabels();
}

void MainWindow::onRefreshDatabases()
{
    if (!m_sql.isConnected()) {
        appendLog(tr("Please connect the database first"));
        return;
    }
    const QStringList dbs = m_sql.listDatabases();
    m_ui->databaseCombo->clear();
    m_ui->databaseCombo->addItems(dbs);
    appendLog(tr("Databases refreshed:") + QLatin1Char(' ') + QString::number(dbs.size()));
}

void MainWindow::onUseDatabase()
{
    const QString databaseName = currentDatabaseName();
    if (databaseName.isEmpty()) {
        appendLog(tr("Database name cannot be empty"));
        return;
    }
    m_ui->dbName->setText(databaseName);
    if (!m_sql.isConnected()) {
        appendLog(tr("Database name prepared:") + QLatin1Char(' ') + databaseName);
        return;
    }
    if (m_sql.useDatabase(databaseName)) {
        appendLog(tr("Using database:") + QLatin1Char(' ') + databaseName);
        onRefreshTables();
    } else {
        appendLog(tr("Use database failed:") + QLatin1Char(' ') + m_sql.lastError());
    }
}

void MainWindow::onDatabaseComboActivated(int)
{
    m_ui->dbName->setText(m_ui->databaseCombo->currentText().trimmed());
    onUseDatabase();
}

void MainWindow::onRefreshTables()
{
    if (!m_sql.isConnected()) {
        appendLog(tr("Please connect the database first"));
        return;
    }
    const QString databaseName = currentDatabaseName();
    if (databaseName.isEmpty()) {
        appendLog(tr("Please choose a database first"));
        return;
    }
    if (!m_sql.useDatabase(databaseName)) {
        appendLog(tr("Use database failed:") + QLatin1Char(' ') + m_sql.lastError());
        return;
    }
    const QString previousTable = currentTableName();
    m_ui->dbName->setText(databaseName);
    const QStringList tables = m_sql.listTables(databaseName);
    m_ui->tableCombo->blockSignals(true);
    m_ui->tableCombo->clear();
    m_ui->tableCombo->addItems(tables);
    if (!previousTable.isEmpty()) {
        const int idx = tables.indexOf(previousTable);
        if (idx >= 0)
            m_ui->tableCombo->setCurrentIndex(idx);
        else
            m_ui->tableCombo->setEditText(previousTable);
    }
    m_ui->tableCombo->blockSignals(false);
    syncBridgeSettings();
    updateTableContextLabels();
    refreshConnectionLabels();
    appendLog(tr("Tables refreshed for") + QLatin1Char(' ') + databaseName + QLatin1String(": ") + QString::number(tables.size()));
}

void MainWindow::onCreateLogTable()
{
    const QString tableName = currentTableName();
    appendLog(QStringLiteral("[widget] createLogTable() clicked"));
    syncBridgeSettings();
    if (!m_sql.isConnected()) {
        appendLog(tr("Please connect the database first"));
        return;
    }
    if (tableName.isEmpty()) {
        appendLog(tr("Please choose or enter a target table first"));
        return;
    }
    if (!ensureWorkingDatabase())
        return;
    if (currentTargetTableExists()) {
        appendLog(tr("Create target table skipped: table already exists"));
        showInfoDialog(tr("Table already exists"),
                       tr("The current target table already exists in the selected database. "
                          "Please choose another table name or use the existing table directly."));
        return;
    }
    if (m_bridge.createDefaultMqttLogTable(tableName)) {
        appendLog(tr("Target table is ready:") + QLatin1Char(' ') + tableName);
        onRefreshTables();
        m_ui->tableCombo->setCurrentText(tableName);
        syncBridgeSettings();
        updateTableContextLabels();
    } else {
        appendLog(tr("Create log table failed:") + QLatin1Char(' ') + m_sql.lastError());
    }
}

void MainWindow::onAddSubscription()
{
    const QString topic = m_ui->topicFilter->text().trimmed();
    const QString tableName = currentTableName();
    appendLog(QStringLiteral("[widget] addSubscription() clicked"));
    if (topic.isEmpty()) {
        appendLog(tr("Topic filter cannot be empty"));
        return;
    }
    if (!m_mqtt.isConnected()) {
        appendLog(tr("Please connect MQTT before subscribing"));
        showInfoDialog(tr("MQTT not connected"), tr("Connect to the MQTT broker before subscribing to a topic."));
        return;
    }
    if (!m_sql.isConnected()) {
        appendLog(tr("Please connect the database first"));
        return;
    }
    if (tableName.isEmpty()) {
        appendLog(tr("Please choose or enter a target table first"));
        return;
    }
    if (!ensureWorkingDatabase())
        return;
    syncBridgeSettings();
    if (m_bridge.subscribeLogTopic(topic)) {
        appendLog(tr("Subscribed topic:") + QLatin1Char(' ') + topic);
        m_ui->topicFilter->clear();
    } else {
        appendLog(tr("Subscribe failed. Check the topic filter format."));
    }
}

void MainWindow::onRemoveSubscription()
{
    QListWidgetItem *item = m_ui->subscriptionList->currentItem();
    if (!item)
        return;
    m_bridge.unsubscribeLogTopic(item->text());
}

void MainWindow::onLogSubscriptionsChanged()
{
    updateSubscriptionList();
}

void MainWindow::onCreateTableFromUi()
{
    const QString tableName = m_ui->newTableName->text().trimmed();
    const QString columnsText = m_ui->newTableColumns->toPlainText().trimmed();
    if (tableName.isEmpty() || columnsText.isEmpty()) {
        appendLog(tr("Table name and column definitions are required"));
        return;
    }
    const QString sql = QStringLiteral("CREATE TABLE IF NOT EXISTS `%1` (\n%2\n)").arg(tableName, columnsText);
    m_ui->sqlEditor->setPlainText(sql);
    onExecuteRawSql();
}

void MainWindow::onExecuteRawSql()
{
    QString sql = m_ui->sqlEditor->toPlainText().trimmed();
    if (sql.isEmpty()) {
        appendLog(tr("SQL text cannot be empty"));
        return;
    }
    appendLog(QStringLiteral("[widget] executeRawSql() ") + sql);
    static const QRegularExpression re(
        QStringLiteral(R"(^(select|show|desc|describe|explain|with)\b)"),
        QRegularExpression::CaseInsensitiveOption);
    const bool isQuery = re.match(sql).hasMatch();
    if (isQuery) {
        const QVariantList rows = m_sql.query(sql);
        setSqlResultText(formatRows(rows));
        appendLog(tr("Query executed, rows:") + QLatin1Char(' ') + QString::number(rows.size()));
    } else {
        if (m_sql.exec(sql)) {
            setSqlResultText(tr("Execution succeeded."));
            appendLog(tr("SQL executed successfully"));
            onRefreshDatabases();
            if (!currentDatabaseName().isEmpty())
                onRefreshTables();
        } else {
            setSqlResultText(tr("Execution failed:\n") + m_sql.lastError());
            appendLog(tr("SQL execution failed:") + QLatin1Char(' ') + m_sql.lastError());
        }
    }
}

void MainWindow::onLoadSelectedTableSql()
{
    const QString tableName = currentTableName();
    if (tableName.isEmpty()) {
        appendLog(tr("Please choose a table first"));
        return;
    }
    m_ui->sqlEditor->setPlainText(QStringLiteral("SELECT * FROM `%1` LIMIT 50").arg(tableName));
}

void MainWindow::onRunCrud()
{
    const QString operation = m_ui->crudOp->currentText();
    const QString tableName = currentTableName();
    if (tableName.isEmpty()) {
        appendLog(tr("Please choose a table first"));
        return;
    }
    appendLog(QStringLiteral("[widget] runCrudHelper() op=") + operation + QStringLiteral(" table=") + tableName);

    if (operation == QStringLiteral("Select")) {
        const QString fields = m_ui->crudFields->text().trimmed().isEmpty() ? QStringLiteral("*") : m_ui->crudFields->text().trimmed();
        const QVariantList rows = m_sql.select(
            tableName,
            fields,
            m_ui->crudCondition->text().trimmed(),
            m_ui->crudOrderBy->text().trimmed(),
            parseIntOrDefault(m_ui->crudLimit->text(), -1),
            0);
        setSqlResultText(formatRows(rows));
        appendLog(tr("Select executed, rows:") + QLatin1Char(' ') + QString::number(rows.size()));
        return;
    }

    if (operation == QStringLiteral("Insert")) {
        const QVariantMap insertValues = parseKeyValueLines(m_ui->crudValues->toPlainText());
        if (m_sql.insertRecord(tableName, insertValues)) {
            setSqlResultText(tr("Insert succeeded."));
            appendLog(tr("Insert succeeded"));
        } else {
            setSqlResultText(tr("Insert failed:\n") + m_sql.lastError());
            appendLog(tr("Insert failed:") + QLatin1Char(' ') + m_sql.lastError());
        }
        return;
    }

    if (operation == QStringLiteral("Update")) {
        const QVariantMap updateValues = parseKeyValueLines(m_ui->crudValues->toPlainText());
        if (m_sql.updateRecord(tableName, updateValues, m_ui->crudCondition->text().trimmed())) {
            setSqlResultText(tr("Update succeeded."));
            appendLog(tr("Update succeeded"));
        } else {
            setSqlResultText(tr("Update failed:\n") + m_sql.lastError());
            appendLog(tr("Update failed:") + QLatin1Char(' ') + m_sql.lastError());
        }
        return;
    }

    if (operation == QStringLiteral("Delete")) {
        if (m_sql.remove(tableName, m_ui->crudCondition->text().trimmed())) {
            setSqlResultText(tr("Delete succeeded."));
            appendLog(tr("Delete succeeded"));
        } else {
            setSqlResultText(tr("Delete failed:\n") + m_sql.lastError());
            appendLog(tr("Delete failed:") + QLatin1Char(' ') + m_sql.lastError());
        }
    }
}
