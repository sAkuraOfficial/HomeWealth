// 客户端
#pragma once
#include <qdebug.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qwebsocket.h>

class Protocol : public QObject
{
    Q_OBJECT
  public:
    Protocol(QObject *parent = nullptr);
    ~Protocol();
    QWebSocket *getWebSocket();
    void ConnectToServer(QString ws);
    void sendMessage(QString msg);
    void disconnect_to_server(); // 主动断开连接

  private:
    QWebSocket *m_pWebSocket;
    QString m_ws;
  private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onTextMessageReceived(QString message);
  signals:
    void ReceiveNewMessage(QString message);

};