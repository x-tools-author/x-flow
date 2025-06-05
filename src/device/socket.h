﻿/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QPair>

#include "device.h"

class Socket : public Device
{
    Q_OBJECT
public:
    explicit Socket(QObject *parent = nullptr);
    void load(const QJsonObject &parameters) override;
    void setDataChannel(int channel);

protected:
    quint16 m_serverPort{12347};
    QString m_serverAddress{"127.0.0.1"};

    int m_channel;
    bool m_authentication{false};
    QString m_username{""};
    QString m_password{""};

    quint16 m_multicastPort{1024};
    QString m_multicastAddress{"239.168.3.255"};
    bool m_enableMulticast{false};
    bool m_justMulticast{false};

protected:
    QString makeFlag(const QString &address, quint16 port) const;
    QPair<QString, quint16> splitFlag(const QString &flag) const;
    bool isValidFlag(const QPair<QString, quint16> &pair) const;
};
