﻿/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "websocketclient.h"

#include "common/common.h"

WebSocketClient::WebSocketClient(QObject *parent)
    : SocketClient(parent)
{}

WebSocketClient::~WebSocketClient() {}

QObject *WebSocketClient::initDevice()
{
    m_webSocket = new QWebSocket();
    connect(m_webSocket,
            &QWebSocket::textMessageReceived,
            m_webSocket,
            [this](const QString &message) { onTextMessageReceived(message); });
    connect(m_webSocket,
            &QWebSocket::binaryMessageReceived,
            m_webSocket,
            [this](const QByteArray &message) { onBinaryMessageReceived(message); });
    connect(m_webSocket, &QWebSocket::disconnected, m_webSocket, [this]() {
        emit errorOccurred("");
    });
    connect(m_webSocket, xWebSocketErrorOccurred, m_webSocket, [this]() {
        emit errorOccurred(m_webSocket->errorString());
    });

    QString url = QString("ws://%1:%2").arg(m_serverAddress).arg(m_serverPort);
    if (m_authentication) {
        QNetworkRequest request(url);
        QString username = m_username;
        QString password = m_password;
        QString concatenated = username + ":" + password;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        QString headerData = "Basic " + data;
        request.setRawHeader("Authorization", headerData.toLocal8Bit());

        qInfo() << "Request: " << request.url();
        qInfo() << "User: " << username << " Password: " << password;

        m_webSocket->open(request);
    } else {
        m_webSocket->open(url);
    }
    return m_webSocket;
}

void WebSocketClient::deinitDevice()
{
    m_webSocket->close();
    m_webSocket->deleteLater();
    m_webSocket = nullptr;
}

void WebSocketClient::writeActually(const QByteArray &bytes)
{
    if (m_channel == static_cast<int>(WebSocketDataChannel::Text)) {
        if (m_webSocket->sendTextMessage(QString::fromUtf8(bytes)) > 0) {
            emit bytesWritten(bytes, makeFlag(m_serverAddress, m_serverPort) + "[T]");
        }
    } else if (m_channel == static_cast<int>(WebSocketDataChannel::Binary)) {
        if (m_webSocket->sendBinaryMessage(bytes) > 0) {
            emit bytesWritten(bytes, makeFlag(m_serverAddress, m_serverPort) + "[B]");
        }
    } else {
        qWarning() << "Invalid data channel: " << m_channel;
    }
}

void WebSocketClient::onTextMessageReceived(const QString &message)
{
    QString flag = makeFlag(m_serverAddress, m_serverPort);
    emit bytesRead(message.toUtf8(), flag + "[T]");
}

void WebSocketClient::onBinaryMessageReceived(const QByteArray &message)
{
    QString flag = makeFlag(m_serverAddress, m_serverPort);
    emit bytesRead(message, flag + "[B]");
}
