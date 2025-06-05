/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of eTools project.
 *
 * eTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "localsocketui.h"
#include "ui_localsocketui.h"

#include "localsocket.h"

LocalSocketUi::LocalSocketUi(QWidget *parent)
    : DeviceUi(parent)
    , ui(new Ui::LocalSocketUi)
{
    ui->setupUi(this);
    ui->lineEditServerName->setText(QString("localhost"));
    connect(ui->lineEditServerName,
            &QLineEdit::textChanged,
            this,
            &LocalSocketUi::parametersChanged);
}

LocalSocketUi::~LocalSocketUi() {}

QJsonObject LocalSocketUi::save() const
{
    QJsonObject parameters = DeviceUi::save();
    LocalSocketParametersKeys keys;
    parameters[keys.serverName] = ui->lineEditServerName->text();
    return parameters;
}

void LocalSocketUi::load(const QJsonObject &parameters)
{
    DeviceUi::load(parameters);
    LocalSocketParametersKeys keys;
    QString defaultServerName = QString("localhost");
    QString serverName = parameters.value(keys.serverName).toString(defaultServerName);
    if (serverName.isEmpty()) {
        serverName = defaultServerName;
    }
    ui->lineEditServerName->setText(serverName);
}

Device *LocalSocketUi::newDevice()
{
    return new LocalSocket(this);
}