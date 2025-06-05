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
class ChartsTestUi;
}
QT_END_NAMESPACE

class ChartsTestUi : public DeviceUi
{
    Q_OBJECT
public:
    ChartsTestUi(QWidget *parent = nullptr);

    Device *newDevice() override;
    QJsonObject save() const override;
    void load(const QJsonObject &parameters) override;

private:
    Ui::ChartsTestUi *ui;
};
