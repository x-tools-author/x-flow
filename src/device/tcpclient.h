﻿/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QTcpSocket>

#include "socketclient.h"

class TcpClient : public SocketClient
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient() override;

    QObject *initDevice() override;
    void deinitDevice() override;
    void writeActually(const QByteArray &bytes) override;

private:
    QTcpSocket *m_tcpSocket{nullptr};

private:
    void readBytesFromDevice();
};
