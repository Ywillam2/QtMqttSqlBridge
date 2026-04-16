#ifndef MQTTMANAGER_H
#define MQTTMANAGER_H

#include <QObject>
#include <QString>
#include <QMqttClient>
#include <QMqttSubscription>
#include <QVariantList>
#include <QHash>

class MqttManager : public QObject
{
    Q_OBJECT

public:
    explicit MqttManager(QObject *parent = nullptr);
    ~MqttManager();

    Q_INVOKABLE void connectToBroker(const QString &host, int port, const QString &clientId, const QString &username, const QString &password);
    Q_INVOKABLE void disconnectFromBroker();
    Q_INVOKABLE void subscribe(const QString &topic);
    Q_INVOKABLE void unsubscribe(const QString &topic);
    Q_INVOKABLE void publish(const QString &topic, const QString &message);

    Q_INVOKABLE bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &topic, const QString &message, int qos, bool retained, bool duplicated);
    void error(const QString &errorMessage);

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorChanged(QMqttClient::ClientError error);

private:
    QMqttClient *m_client;
    QHash<QString, QMqttSubscription *> m_subscriptions;
};

#endif // MQTTMANAGER_H