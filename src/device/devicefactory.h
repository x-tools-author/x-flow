/***************************************************************************************************
 * Copyright 2025-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include <QObject>

class DeviceUi;
class DeviceFactory : public QObject
{
    Q_OBJECT
private:
    explicit DeviceFactory(QObject *parent = nullptr);

public:
    ~DeviceFactory();

    static DeviceFactory &singleton();

    QList<int> supportedDeviceTypes();
    QString deviceName(int type);
    QString deviceRawName(int type);
    DeviceUi *newDeviceUi(int type);
};
