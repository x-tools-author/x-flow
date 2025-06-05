/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of eTools project.
 *
 * eTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "localsocket.h"

LocalSocket::LocalSocket(QObject *parent)
    : Device(parent)
{}

LocalSocket::~LocalSocket() {}

QObject *LocalSocket::initDevice()
{
    m_socket = new QLocalSocket();
    connect(m_socket, &QLocalSocket::connected, m_socket, [this]() { emit opened(); });
    connect(m_socket, &QLocalSocket::disconnected, m_socket, [this]() { emit closed(); });
    connect(m_socket, &QLocalSocket::readyRead, m_socket, [this]() {
        QByteArray bytes = this->m_socket->readAll();
        emit bytesRead(bytes, m_socket->serverName());
    });
    connect(m_socket,
            &QLocalSocket::errorOccurred,
            m_socket,
            [this](QLocalSocket::LocalSocketError error) {
                emit errorOccurred(m_socket->errorString());
                emit closed();
            });

    QJsonObject parameters = save();
    LocalSocketParametersKeys keys;
    QString socketName = parameters.value(keys.serverName).toString();
    if (socketName.isEmpty()) {
        m_socket->deleteLater();
        m_socket = nullptr;
        return nullptr;
    }

    m_socket->connectToServer(socketName);
    if (!m_socket->waitForConnected()) {
        m_socket->deleteLater();
        m_socket = nullptr;
        return nullptr;
    }

    return m_socket;
}

void LocalSocket::deinitDevice()
{
    if (m_socket) {
        m_socket->disconnectFromServer();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

void LocalSocket::writeActually(const QByteArray &bytes)
{
    if (m_socket && m_socket->state() == QLocalSocket::ConnectedState) {
        qint64 ret = m_socket->write(bytes);
        if (ret == bytes.size()) {
            emit bytesWritten(bytes, m_socket->serverName());
        }
    } else {
        emit errorOccurred("Socket is not connected");
    }
}