/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of eTools project.
 *
 * eTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "localserverui.h"
#include "ui_localserverui.h"

#include "common/common.h"
#include "localserver.h"

LocalServerUi::LocalServerUi(QWidget *parent)
    : DeviceUi(parent)
    , ui(new Ui::LocalServerUi)
{
    ui->setupUi(this);
    ui->lineEditServerName->setText(QString("localhost"));
    ui->comboBoxClients->addItem(tr("All"), 0);
    connect(ui->comboBoxClients, xComboBoxActivated, this, &LocalServerUi::onTargetChanged);
    connect(ui->lineEditServerName,
            &QLineEdit::textChanged,
            this,
            &LocalServerUi::parametersChanged);
}

LocalServerUi::~LocalServerUi() {}

QJsonObject LocalServerUi::save() const
{
    QJsonObject parameters = DeviceUi::save();
    LocalServerParametersKeys keys;
    parameters[keys.serverName] = ui->lineEditServerName->text();
    return parameters;
}

void LocalServerUi::load(const QJsonObject &parameters)
{
    DeviceUi::load(parameters);
    LocalServerParametersKeys keys;
    QString defaultServerName = QString("localhost");
    QString serverName = parameters.value(keys.serverName).toString(defaultServerName);
    if (serverName.isEmpty()) {
        serverName = defaultServerName;
    }
    ui->lineEditServerName->setText(serverName);
}

void LocalServerUi::setUiEnabled(bool enabled)
{
    ui->lineEditServerName->setEnabled(enabled);
}

Device *LocalServerUi::newDevice()
{
    m_server = new LocalServer(this);
    connect(m_server, &LocalServer::socketConnected, this, &LocalServerUi::onSocketConnected);
    connect(m_server, &LocalServer::socketDisconnected, this, &LocalServerUi::onSocketDisconnected);
    return m_server;
}

void LocalServerUi::onSocketConnected(QLocalSocket *socket, const QString &socketName)
{
    ui->comboBoxClients->addItem(socketName, QVariant::fromValue(socket));
}

void LocalServerUi::onSocketDisconnected(QLocalSocket *socket)
{
    int index = ui->comboBoxClients->findData(QVariant::fromValue(socket));
    if (index != -1) {
        ui->comboBoxClients->removeItem(index);
    }
}

void LocalServerUi::onTargetChanged()
{
    QLocalSocket *socket = ui->comboBoxClients->currentData().value<QLocalSocket *>();
    m_server->setTarget(socket);
}