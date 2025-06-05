/***************************************************************************************************
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
class SerialPortUi;
}
QT_END_NAMESPACE

class SerialPortScanner;
class SerialPortUi : public DeviceUi
{
    Q_OBJECT
public:
    SerialPortUi(QWidget *parent = nullptr);

    QJsonObject save() const override;
    void load(const QJsonObject &parameters) override;

protected:
    Device *newDevice() override;

private:
    void refresh();

private:
    Ui::SerialPortUi *ui;
};
