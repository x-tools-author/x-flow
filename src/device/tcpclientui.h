﻿/***************************************************************************************************
 * Copyright 2024-2025 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xFlow project.
 *
 * xFlow is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#pragma once

#include "socketclientui.h"

class TcpClientUi : public SocketClientUi
{
    Q_OBJECT
public:
    explicit TcpClientUi(QWidget *parent = nullptr);
    ~TcpClientUi() override;

    Device *newDevice() override;
    void setUiEnabled(bool enabled) override;
};
