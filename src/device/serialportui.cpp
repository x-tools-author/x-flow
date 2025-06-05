/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "serialportui.h"
#include "ui_serialportui.h"

#include <QSerialPort>
#include <QSerialPortInfo>

#include "common/common.h"
#include "serialport.h"

SerialPortUi::SerialPortUi(QWidget *parent)
    : DeviceUi(parent)
    , ui(new Ui::SerialPortUi)
{
    ui->setupUi(this);
    ui->checkBoxOptimizedFrame->setChecked(true);
#if defined(Q_OS_LINUX)
    ui->comboBoxPortName->setEditable(true);
#endif

    setupBaudRate(ui->comboBoxBaudRate);
    setupDataBits(ui->comboBoxDataBits);
    setupParity(ui->comboBoxParity);
    setupStopBits(ui->comboBoxStopBits);
    setupFlowControl(ui->comboBoxFlowControl);

    // clang-format off
    connect(ui->comboBoxPortName, &QComboBox::currentIndexChanged, this, &SerialPortUi::parametersChanged);
    connect(ui->comboBoxBaudRate, &QComboBox::currentIndexChanged, this, &SerialPortUi::parametersChanged);
    connect(ui->comboBoxDataBits, &QComboBox::currentIndexChanged, this, &SerialPortUi::parametersChanged);
    connect(ui->comboBoxParity, &QComboBox::currentIndexChanged, this, &SerialPortUi::parametersChanged);
    connect(ui->comboBoxStopBits, &QComboBox::currentIndexChanged, this, &SerialPortUi::parametersChanged);
    connect(ui->comboBoxFlowControl, &QComboBox::currentIndexChanged, this, &SerialPortUi::parametersChanged);
    connect(ui->checkBoxOptimizedFrame, &QCheckBox::clicked, this, &SerialPortUi::parametersChanged);
    connect(ui->pushButtonRefreshDevice, &QPushButton::clicked, this, &SerialPortUi::refresh);
    // clang-format on

    refresh();
}

QJsonObject SerialPortUi::save() const
{
    QJsonObject parameters;
    SerialPortItemKeys keys;
    parameters[keys.portName] = ui->comboBoxPortName->currentText();
    parameters[keys.baudRate] = ui->comboBoxBaudRate->currentText().toInt();
    parameters[keys.dataBits] = ui->comboBoxDataBits->currentData().toInt();
    parameters[keys.parity] = ui->comboBoxParity->currentData().toInt();
    parameters[keys.stopBits] = ui->comboBoxStopBits->currentData().toInt();
    parameters[keys.flowControl] = ui->comboBoxFlowControl->currentData().toInt();
    parameters[keys.optimizedFrame] = ui->checkBoxOptimizedFrame->isChecked();
    return parameters;
}

void SerialPortUi::load(const QJsonObject &parameters)
{
    SerialPortItemKeys keys;
    QVariantMap map = parameters.toVariantMap();
    QString portName = map.value(keys.portName).toString();
    int baudRate = map.value(keys.baudRate, 9600).toInt();
    int dataBits = map.value(keys.dataBits, static_cast<int>(QSerialPort::Data8)).toInt();
    int parity = map.value(keys.parity, static_cast<int>(QSerialPort::NoParity)).toInt();
    int stopBits = map.value(keys.stopBits, static_cast<int>(QSerialPort::OneStop)).toInt();
    int fc = map.value(keys.flowControl, static_cast<int>(QSerialPort::NoFlowControl)).toInt();
    bool optimizedFrame = map.value(keys.optimizedFrame, true).toBool();

    ui->comboBoxPortName->setCurrentText(portName);
    ui->comboBoxBaudRate->setCurrentText(QString::number(baudRate));
    ui->comboBoxDataBits->setCurrentIndex(ui->comboBoxDataBits->findData(dataBits));
    ui->comboBoxParity->setCurrentIndex(ui->comboBoxParity->findData(parity));
    ui->comboBoxStopBits->setCurrentIndex(ui->comboBoxStopBits->findData(stopBits));
    ui->comboBoxFlowControl->setCurrentIndex(ui->comboBoxFlowControl->findData(fc));
    ui->checkBoxOptimizedFrame->setChecked(optimizedFrame);
}

Device *SerialPortUi::newDevice()
{
    return new SerialPort(this);
}

void SerialPortUi::refresh()
{
    QString com = ui->comboBoxPortName->currentText();
    setupPortName(ui->comboBoxPortName);

    int index = ui->comboBoxPortName->findText(com);
    if (index >= 0) {
        ui->comboBoxPortName->setCurrentIndex(index);
    } else {
        ui->comboBoxPortName->setCurrentIndex(0);
    }
}
