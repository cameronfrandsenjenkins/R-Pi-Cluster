#include "TCPSenderWeb.hpp"
#include "Logger/Logger.hpp"

#include <iostream>
#include <qbytearray.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

TCPSenderWeb::TCPSenderWeb()
  : QObject(nullptr), m_convId(1), m_pServer(new QTcpServer(this)), m_pSocket()
{
  Logger::info("waiting for connection");
  if (!m_pServer->listen())
  {
    Logger::info("not listening");
    return;
  }
  connect(m_pServer.get(),
          &QTcpServer::newConnection,
          this,
          &TCPSenderWeb::connection);
}

TCPSenderWeb::~TCPSenderWeb() {}

qint64 TCPSenderWeb::send(std::string msg)
{
  Logger::info("Sending to Web: " + msg);

  return m_pSocket->write(
    QByteArray(msg.c_str(), static_cast<int>(msg.size())));
}

quint16 TCPSenderWeb::getServerPort()
{
  return m_pServer->serverPort();
}

int TCPSenderWeb::nextConvId()
{
  return m_convId++;
}

void TCPSenderWeb::connection()
{
  m_pSocket = std::shared_ptr<QTcpSocket>(m_pServer->nextPendingConnection());

  connect(
    m_pSocket.get(), &QIODevice::readyRead, this, &TCPSenderWeb::emitMessage);
  connect(m_pSocket.get(),
          &QTcpSocket::disconnected,
          this,
          &TCPSenderWeb::disconnected);
  Logger::info("Connection Successful to Web");
  // send id
  emit newConnection();
}

void TCPSenderWeb::emitMessage()
{
  QTcpSocket* readSocket = qobject_cast<QTcpSocket*>(sender());
  auto str = readSocket->readAll().toStdString();
  std::string msg;
  while (!str.empty())
  {
    auto spot = str.find("~");
    if (spot == std::string::npos) spot = str.size();
    msg = str.substr(0, spot);
    str.erase(0, spot + 1);
    emit msgReceived(msg);
  }
}

void TCPSenderWeb::disconnected()
{
  emit lostConnection();
}
