#ifndef MQTTDBTESTBRIDGE_H
#define MQTTDBTESTBRIDGE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMqttTopicFilter>

class MqttManager;
class SqlManagerWrapper;

class MqttDbTestBridge : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString commandTopic READ commandTopic WRITE setCommandTopic NOTIFY commandTopicChanged)
    Q_PROPERTY(QString responseTopic READ responseTopic WRITE setResponseTopic NOTIFY responseTopicChanged)
    Q_PROPERTY(QString messageLogTable READ messageLogTable WRITE setMessageLogTable NOTIFY messageLogTableChanged)
    Q_PROPERTY(QStringList logSubscriptions READ logSubscriptions NOTIFY logSubscriptionsChanged)

public:
    explicit MqttDbTestBridge(MqttManager *mqtt, SqlManagerWrapper *sql, QObject *parent = nullptr);

    QString commandTopic() const { return m_commandTopic; }
    void setCommandTopic(const QString &t);

    QString responseTopic() const { return m_responseTopic; }
    void setResponseTopic(const QString &t);

    QString messageLogTable() const { return m_messageLogTable; }
    void setMessageLogTable(const QString &t);

    QStringList logSubscriptions() const;

    Q_INVOKABLE bool subscribeLogTopic(const QString &topicFilter);
    Q_INVOKABLE bool unsubscribeLogTopic(const QString &topicFilter);
    Q_INVOKABLE bool createDefaultMqttLogTable(const QString &tableName);

signals:
    void commandTopicChanged();
    void responseTopicChanged();
    void messageLogTableChanged();
    void logSubscriptionsChanged();

private slots:
    void onMqttConnected();
    void onMqttMessage(const QString &topic, const QString &message, int qos, bool retained, bool duplicated);

private:
    void subscribeAllTopics();
    bool handleCommandJson(const QString &message);
    void publishResponse(const QJsonObject &obj);
    bool topicMatchesLogFilter(const QString &topic) const;
    bool insertMessageLog(const QString &topic, const QString &message, int qos, bool retained, bool duplicated);
    static QMap<QString, QVariant> jsonObjectToVariantMap(const QJsonObject &o);
    static QJsonValue variantToJson(const QVariant &v);
    static QJsonArray variantListToJson(const QVariantList &list);

    MqttManager *m_mqtt = nullptr;
    SqlManagerWrapper *m_sql = nullptr;
    QString m_commandTopic;
    QString m_responseTopic;
    QString m_messageLogTable;
    QList<QMqttTopicFilter> m_logTopicFilters;
};

#endif // MQTTDBTESTBRIDGE_H
