/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "devicefactory.h"

#include "device/localserverui.h"
#include "device/localsocketui.h"
#include "device/serialportui.h"
#include "device/tcpclientui.h"
#include "device/tcpserverui.h"
#include "device/udpclientui.h"
#include "device/udpserverui.h"
#include "device/websocketclientui.h"
#include "device/websocketserverui.h"

enum class DeviceType {
    SerialPort,
    BleCentral,
    BlePeripheral,
    UdpClient,
    UdpServer,
    TcpClient,
    TcpServer,
    WebSocketClient,
    WebSocketServer,
    LocalSocket,
    LocalServer,
    //----------------------------------------------------------------------------------------------
    Hid = 0x00200000,
    SctpClient,
    SctpServer,
    //----------------------------------------------------------------------------------------------
    ChartsTest = 0x00300000
};

DeviceFactory::DeviceFactory(QObject* parent)
    : QObject(parent)
{}

DeviceFactory::~DeviceFactory() {}

DeviceFactory& DeviceFactory::singleton()
{
    static DeviceFactory instance;
    return instance;
}

QList<int> DeviceFactory::supportedDeviceTypes()
{
    QList<int> types;
    types << static_cast<int>(DeviceType::SerialPort);
    types << static_cast<int>(DeviceType::UdpClient);
    types << static_cast<int>(DeviceType::UdpServer);
    types << static_cast<int>(DeviceType::TcpClient);
    types << static_cast<int>(DeviceType::TcpServer);
    types << static_cast<int>(DeviceType::WebSocketClient);
    types << static_cast<int>(DeviceType::WebSocketServer);
    types << static_cast<int>(DeviceType::LocalSocket);
    types << static_cast<int>(DeviceType::LocalServer);
    return types;
}

QString DeviceFactory::deviceName(int type)
{
    switch (type) {
    case static_cast<int>(DeviceType::SerialPort):
        return tr("Serial Port");
    case static_cast<int>(DeviceType::BleCentral):
        return tr("BLE Central");
    case static_cast<int>(DeviceType::BlePeripheral):
        return tr("BLE Peripheral");
    case static_cast<int>(DeviceType::UdpClient):
        return tr("UDP Client");
    case static_cast<int>(DeviceType::UdpServer):
        return tr("UDP Server");
    case static_cast<int>(DeviceType::TcpClient):
        return tr("TCP Client");
    case static_cast<int>(DeviceType::TcpServer):
        return tr("TCP Server");
    case static_cast<int>(DeviceType::WebSocketClient):
        return tr("WebSocket Client");
    case static_cast<int>(DeviceType::WebSocketServer):
        return tr("WebSocket Server");
    case static_cast<int>(DeviceType::LocalSocket):
        return tr("Local Socket");
    case static_cast<int>(DeviceType::LocalServer):
        return tr("Local Server");
    case static_cast<int>(DeviceType::ChartsTest):
        return tr("Charts Test");
    default:
        qWarning() << "deviceName() error: Unknown device type" << type;
        return QString("UnknownDeviceName");
    }
}

QString DeviceFactory::deviceRawName(int type)
{
    switch (type) {
    case static_cast<int>(DeviceType::SerialPort):
        return QString("SerialPort");
    case static_cast<int>(DeviceType::BleCentral):
        return QString("BleCentral");
    case static_cast<int>(DeviceType::BlePeripheral):
        return QString("BlePeripheral");
    case static_cast<int>(DeviceType::TcpClient):
        return QString("TcpClient");
    case static_cast<int>(DeviceType::TcpServer):
        return QString("TcpServer");
    case static_cast<int>(DeviceType::UdpClient):
        return QString("UdpClient");
    case static_cast<int>(DeviceType::UdpServer):
        return QString("UdpServer");
    case static_cast<int>(DeviceType::WebSocketClient):
        return QString("WebSocketClient");
    case static_cast<int>(DeviceType::WebSocketServer):
        return QString("WebSocketServer");
    case static_cast<int>(DeviceType::LocalSocket):
        return QString("LocalSocket");
    case static_cast<int>(DeviceType::LocalServer):
        return QString("LocalServer");
    case static_cast<int>(DeviceType::ChartsTest):
        return QString("ChartsTest");
    default:
        qWarning() << "deviceRawName() error: Unknown device type" << type;
        return QString("UnknownDeviceRawName");
    }
}

DeviceUi* DeviceFactory::newDeviceUi(int type)
{
    if (type == static_cast<int>(DeviceType::SerialPort)) {
        return new SerialPortUi();
    } else if (type == static_cast<int>(DeviceType::TcpClient)) {
        return new TcpClientUi();
    } else if (type == static_cast<int>(DeviceType::TcpServer)) {
        return new TcpServerUi();
    } else if (type == static_cast<int>(DeviceType::UdpClient)) {
        return new UdpClientUi();
    } else if (type == static_cast<int>(DeviceType::UdpServer)) {
        return new UdpServerUi();
    } else if (type == static_cast<int>(DeviceType::WebSocketClient)) {
        return new WebSocketClientUi();
    } else if (type == static_cast<int>(DeviceType::WebSocketServer)) {
        return new WebSocketServerUi();
    } else if (type == static_cast<int>(DeviceType::LocalSocket)) {
        return new LocalSocketUi();
    } else if (type == static_cast<int>(DeviceType::LocalServer)) {
        return new LocalServerUi();
    } else {
        qWarning() << "newDeviceUi() error: Unknown device type" << type;
        return nullptr;
    }
}
