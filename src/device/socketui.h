﻿/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include "deviceui.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class SocketUi;
}
QT_END_NAMESPACE

class Socket;
class SocketUi : public DeviceUi
{
    Q_OBJECT
public:
    SocketUi(QWidget *parent = nullptr);
    ~SocketUi();

    QJsonObject save() const override;
    void load(const QJsonObject &parameters) override;

signals:
    void currentClientChanged(const QString &flag);
    void invokeDisconnectAll();

protected:
    Socket *m_socket{nullptr};

protected:
    void setServerWidgetsVisible(bool visible);
    void setChannelWidgetsVisible(bool visible);
    void setAuthenticationWidgetsVisible(bool visible);
    void setWriteToWidgetsVisible(bool visible);
    void setMulticastWidgetsVisible(bool visible);

    void setServerWidgetsEnabled(bool enabled);
    void setChannelWidgetsEnabled(bool enabled);
    void setAuthenticationWidgetsEnabled(bool enabled);
    void setWriteToWidgetsEnabled(bool enabled);
    void setMulticastWidgetsEnabled(bool enabled);

    void setupClients(const QStringList &clients);

private:
    Ui::SocketUi *ui;
};
